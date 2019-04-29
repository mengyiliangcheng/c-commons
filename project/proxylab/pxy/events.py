############################################################################
#  Proxylab testing framework
#  Event management
############################################################################


import sys
import threading
import datetime
import math
import time
import files

def elapsedSeconds(startTime):
    dt = datetime.datetime.now() - startTime
    return dt.seconds + 1e-6 * dt.microseconds

class EventException(Exception):
    info = ""

    def __init__(self, info = ""):
        Exception.__init__(self)
        self.info = info

    def __str__(self):
        if self.info == "":
            return "Event Error"
        else:
            return "Event Error: %s" % self.info

class Event:
    isRequest = False  # Event is either request or response
    isFetch = False    # Request can be fetch or request
    hasMatch = False   # Set when doing heuristic matching of events
    time = 0.0         # Seconds relative to start of event manager
    id = ""            # Every event has a named ID
    uri = ""           # URI of request
    path = ""          # Path of relevant file
    server = ""        # Id of server
    text = ""          # Diagnostic text
    # What was test outcome?
    #   Possible values: "requesting", "responding", "reading", "closing", "checking", "warning", "error", or an HTTP response status
    tag = ""           
    tevent = None      # Threading event to defer response events
    sockFile = None    # Connected socket for defered request
    url = None         # Request URL
    thread = None      # Thread handling event
    # Headers for tracing.  Given as lists of lines
    pendingHeaderLines = []
    sentHeaderLines = []
    receivedHeaderLines = []

    def __init__(self, isRequest, time, id, path = "", text = "", server = "", isFetch = False):
        self.isRequest = isRequest
        self.isFetch = isFetch
        self.hasMatch = False
        self.time = time
        self.id = id
        self.path = path
        self.server = server
        self.text = text
        self.uri = ""
        self.tag = "ok"
        self.tevent = None
        if not isRequest:
            self.tevent = threading.Event()
            self.tevent.clear()
        self.sockFile = None
        self.url = None
        self.thread = None
        self.pendingHeaderLines = []
        self.sentHeaderLines = []
        self.receivedHeaderLines = []

    def addText(self, text):
        self.text += text

    def addPath(self, path):
        self.path = path

    def addURI(self, uri):
        self.uri = uri

    def setTag(self, tag, reason = None):
        self.tag = tag
        if reason is not None:
            self.text = reason

    def error(self, text = None):
        self.setTag("error", text)

    def warning(self, text = None):
        self.setTag("warning", text)

    def key(self):
        prefix = "request+" if self.isRequest else "response+"
        return self.id
#        return prefix + self.id

    def release(self):
        if self.tevent is not None:
            self.tevent.set()

    def wait(self, timeout = None):
        if self.tevent is not None:
            return self.tevent.wait(timeout)
        else:
            return True

    def startShutdown(self):
        if self.sockFile is not None:
            self.sockFile.shutdown()
        self.release()

    def finishShutdown(self):
        if self.thread is not None:
            self.thread.join()

    def __str__(self):
        stype = "Response"
        if self.isRequest:
            stype = "Fetch" if self.isFetch else "Request"
        stime = "TIME=%.3f" % self.time
        sserver = "" if self.server == "" else " SERVER = %s " % self.server
        suri = "" if self.uri == "" else " URI = %s " % self.uri
        info = "" if self.text == "" else " INFO=%s" % self.text
        pinfo = "" if self.path == "" else " PATH=%s" % self.path
        return "Event[%s %s %s %s%s%s%s%s]" % (stype, self.id, self.tag, stime, sserver, suri, info, pinfo)


  

class EventManager:
    startTime = None  # Time of day when started
    mutex = None      # To ensure that event list managed properly
    requestDict = {}  # Request events, indexed by name
    responseDict = {} # Response events, indexed by name
    list = []         # All events, ordered by time
    # Both sets of events are mappings from event id to event
    waitingEvents = {}   # Events that must be completed before returning to wait command
    completedEvents = {} # Events that must have been completed
    waitEvent = None
    running = True
    heartbeatManager = None # Added when doing wrapped proxy
    heartbeatLimit = 2.0 # How recent a heartbeat to we consider a sign of recent activity (seconds)?

    def __init__(self):
        self.startTime = datetime.datetime.now()
        self.mutex = threading.Lock()
        self.requestDict = {}
        self.list = []
        self.waitingEvents = {}
        self.completedEvents = {}
        self.waitEvent = threading.Event()
        self.waitEvent.clear()
        self.running = True
        self.heartbeatManager = None

    def addRequestEvent(self, id = "", server = "", isFetch = False):
        return self.addEvent(True, id, server = server, isFetch = isFetch)

    def addResponseEvent(self, id = "", server = ""):
        return self.addEvent(False, id, server = server, isFetch = True)

    def addEvent(self, isRequest, id="", server = "", isFetch = False, mutex = True):
        seconds = elapsedSeconds(self.startTime)
        if mutex:
            self.mutex.acquire()

        if not self.running:
            if mutex:
                self.mutex.release()
            return None

        if id == "":
            id = "Event-%d" % (len(self.list) + 1)
        e = Event(isRequest, seconds, id, server = server, isFetch = isFetch)
        e.setTag( "requesting" if isRequest else "responding")
        dict = self.requestDict if isRequest else self.responseDict
        if id in dict:
            self.mutex.release()
            raise EventException("Duplicate %s event %s" % ("request" if isRequest else "response", id))
        dict[id] = e
        self.list.append(e)
        if mutex:
            self.mutex.release()
        return e

    # Mark event as completed
    def addCompleted(self, event):
        if event is None:
            return
        id = event.id
        self.mutex.acquire()
        if id in self.waitingEvents:
            del self.waitingEvents[id]
            if len(self.waitingEvents) == 0:
                self.waitEvent.set()
        else:
            self.completedEvents[id] = event
        self.mutex.release()

    def addBeat(self, origin):
        if self.heartbeatManager is not None:
            msg = "0\t" + origin
            self.heartbeatManager.processBeatMessage(msg)

    def getPending(self):
        self.mutex.acquire()        
        pending = self.waitingEvents.values()
        self.mutex.release()
        return pending

    def waitActive(self, waitingEvents, timeout):
        # Convert from milliseconds to seconds
        tsecs = timeout / 1000.0
        # Break into multiples of heartbeat liit
        wsecs = min(tsecs, self.heartbeatLimit)
        mreps = int(math.ceil(tsecs/wsecs))
        self.mutex.acquire()        
        for id in waitingEvents.keys():
            if id in self.completedEvents:
                del waitingEvents[id]
                del self.completedEvents[id]
        self.waitingEvents = waitingEvents
        if len(self.waitingEvents) == 0:
            self.waitEvent.set()
        else:
            self.waitEvent.clear()
        self.mutex.release()
        waitStart = datetime.datetime.now()
        timeoutReason = "Time limit exceeded"
        for r in range(mreps):
            self.waitEvent.wait(wsecs)
            self.mutex.acquire()
            pending = self.waitingEvents.values()
            self.mutex.release()
            doneWaiting = len(pending) == 0
            # See if there's a heartbeat
            if not doneWaiting and self.heartbeatManager is not None:
                lastBeat = self.heartbeatManager.lastBeat()
                if lastBeat is not None:
                    age = lastBeat.age()
                    self.heartbeatManager.beatAge(age)
                    if age > self.heartbeatLimit:
                        doneWaiting = True
                        timeoutReason = "No proxy activity detected for %.1f ms" % (age * 1000.0)
            if doneWaiting:
                break
        self.mutex.acquire()
        self.waitingEvents = {}
        self.mutex.release()
        if len(pending) == 0:
            return (pending, "")
        else:
            elapsed = elapsedSeconds(waitStart)
            return (pending, "Wait timeout after %.2f seconds: " % elapsed + timeoutReason)
        return pending

    # Safe way to alter tag of active event
    def changeTag(self, event, tag, reason):
        self.mutex.acquire()
        event.setTag(tag, reason)
        self.mutex.release()

    def makeMatchEvent(self, server, uri):
        nevent = None
        self.mutex.acquire()
        for event in self.list:
            if event.isRequest and event.uri == uri and event.server == server and event.tag == "requesting" and not event.hasMatch:
                event.hasMatch = True
                try:
                    nevent = self.addEvent(False, id = event.id, server = server, isFetch = event.isFetch, mutex = False)
                except EventException:
                    nevent = None
                break
        self.mutex.release()
        return nevent

    def delay(self, milliseconds):
        time.sleep(milliseconds * 1e-3)

    def findEvent(self, isRequest, id):
        e = None
        self.mutex.acquire()
        dict = self.requestDict if isRequest else self.responseDict
        if id in dict:
            e = dict[id]
        self.mutex.release()
        return e

    def stop(self):
        self.mutex.acquire()
        self.running = False
        self.mutex.release()

    def shutdown(self):
        self.mutex.acquire()
        ls = list(self.list)
        self.mutex.release()
        for e in ls:
            e.startShutdown()
        for e in ls:
            e.finishShutdown()

    def stringList(self):
        self.mutex.acquire()
        ls = [str(e) for e in self.list]
        self.mutex.release()
        return ls
        
        
