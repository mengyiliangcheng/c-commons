/**
 * @file csapp.c
 * @brief Functions for the CS:APP3e book
 */

#include "csapp.h"

#include <stdio.h>                      /* stderr */
#include <string.h>                     /* memset() */
#include <stdbool.h>                    /* bool */
#include <stdlib.h>                     /* abort() */
#include <stddef.h>                     /* ssize_t */
#include <stdint.h>                     /* intmax_t */
#include <stdarg.h>                     /* va_list */
#include <errno.h>                      /* errno */
#include <unistd.h>                     /* STDIN_FILENO */
#include <semaphore.h>                  /* sem_t */
#include <netdb.h>                      /* freeaddrinfo() */
#include <sys/types.h>                  /* struct sockaddr */
#include <sys/socket.h>                 /* struct sockaddr */
#include <signal.h>                     /* struct sigaction */

/************************************
 * Wrappers for Unix signal functions
 ***********************************/

/**
 * @brief   Wrapper for the new sigaction interface. Exits on error.
 * @param signum    Signal to set handler for.
 * @param handler   Handler function.
 *
 * @return  Previous disposition of the signal.
 */
handler_t *Signal(int signum, handler_t *handler) {
  struct sigaction action, old_action;

  action.sa_handler = handler;
  sigemptyset(&action.sa_mask); /* Block sigs of type being handled */
  action.sa_flags = SA_RESTART; /* Restart syscalls if possible */

  if (sigaction(signum, &action, &old_action) < 0) {
    perror("Signal error");
    exit(1);
  }

  return old_action.sa_handler;
}


/*************************************************************
 * The Sio (Signal-safe I/O) package - simple reentrant output
 * functions that are safe for signal handlers.
 *************************************************************/

/* Private sio functions */

/* sio_reverse - Reverse a string (from K&R) */
static void sio_reverse(char s[], size_t len) {
    size_t i, j;
    for (i = 0, j = len - 1; i < j; i++, j--) {
        char c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* write_digits - write digit values of v in base b to string */
static size_t write_digits(uintmax_t v, char s[], unsigned char b) {
    size_t i = 0;
    do {
        unsigned char c = v % b;
        if (c < 10) {
            s[i++] = c + '0';
        }
        else {
            s[i++] = c - 10 + 'a';
        }
    } while ((v /= b) > 0);
    return i;
}

/* Based on K&R itoa() */
/* intmax_to_string - Convert an intmax_t to a base b string */
static size_t intmax_to_string(intmax_t v, char s[], unsigned char b) {
    bool neg = v < 0;
    size_t len;

    if (neg) {
        len = write_digits(-v, s, b);
        s[len++] = '-';
    }
    else {
        len = write_digits(v, s, b);
    }

    s[len] = '\0';
    sio_reverse(s, len);
    return len;
}

/* uintmax_to_string - Convert a uintmax_t to a base b string */
static size_t uintmax_to_string(uintmax_t v, char s[], unsigned char b) {
    size_t len = write_digits(v, s, b);
    s[len] = '\0';
    sio_reverse(s, len);
    return len;
}

/* Public Sio functions */

/**
 * @brief   Prints formatted output to stdout.
 * @param fmt   The format string used to determine the output.
 * @param ...   The arguments for the format string.
 * @return      The number of bytes written, or -1 on error.
 *
 * @remark   This function is async-signal-safe.
 * @see      sio_vfprintf
 */
ssize_t sio_printf(const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    ssize_t ret = sio_vfprintf(STDOUT_FILENO, fmt, argp);
    va_end(argp);
    return ret;
}

/**
 * @brief   Prints formatted output to a file descriptor.
 * @param fileno   The file descriptor to print output to.
 * @param fmt      The format string used to determine the output.
 * @param ...      The arguments for the format string.
 * @return         The number of bytes written, or -1 on error.
 *
 * @remark   This function is async-signal-safe.
 * @see      sio_vfprintf
 */
ssize_t sio_fprintf(int fileno, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    ssize_t ret = sio_vfprintf(fileno, fmt, argp);
    va_end(argp);
    return ret;
}

/**
 * @brief   Prints formatted output to a file descriptor from a va_list.
 * @param fileno   The file descriptor to print output to.
 * @param fmt      The format string used to determine the output.
 * @param argp     The arguments for the format string.
 * @return         The number of bytes written, or -1 on error.
 *
 * @remark   This function is async-signal-safe.
 *
 * This is a reentrant and async-signal-safe implementation of vfprintf, used
 * to implement the associated formatted sio functions.
 *
 * This function writes directly to a file descriptor (using the `rio_writen`
 * function from csapp), as opposed to a `FILE *` from the standard library.
 * However, since these writes are unbuffered, this is not very efficient, and
 * should only be used when async-signal-safety is necessary.
 *
 * The only supported format specifiers are the following:
 *  -  Int types: %d, %i, %u, %x, %o (with size specifiers l, z)
 *  -  Others: %c, %s, %%, %p
 */
ssize_t sio_vfprintf(int fileno, const char *fmt, va_list argp) {
    size_t pos = 0;
    ssize_t num_written = 0;

    while (fmt[pos] != '\0') {
        // String output of this iteration
        const char *str = NULL;  // String to output
        size_t len = 0;  // Length of string to output

        // Mark whether we've matched a format
        bool handled = false;

        // Int to string conversion
        char buf[128];
        char convert_type = '\0';
        union {
            uintmax_t u;
            intmax_t s;
        } convert_value = {.u = 0};

        // Handle format characters
        if (fmt[pos] == '%') {
            switch (fmt[pos + 1]) {

            // Character format
            case 'c':
                buf[0] = (char) va_arg(argp, int);
                buf[1] = '\0';
                str = buf;
                len = 1;
                handled = true;
                pos += 2;
                break;

            // String format
            case 's':
                str = va_arg(argp, char *);
                len = strlen(str);
                handled = true;
                pos += 2;
                break;

            // Escaped %
            case '%':
                str = &fmt[pos + 1];
                len = 1;
                handled = true;
                pos += 2;
                break;

            // Pointer type
            case 'p': {
                void *ptr = va_arg(argp, void *);
                if (ptr == NULL) {
                    str = "(nil)";
                    len = strlen(str);
                    handled = true;
                }
                else {
                    convert_type = 'p';
                    convert_value.u = (uintmax_t) (uintptr_t) ptr;
                }
                pos += 2;
                break;
            }

            // Int types with no format specifier
            case 'd':
            case 'i':
                convert_type = 'd';
                convert_value.s = (intmax_t) va_arg(argp, int);
                pos += 2;
                break;
            case 'u':
                convert_type = 'u';
                convert_value.u = (uintmax_t) va_arg(argp, unsigned);
                pos += 2;
                break;
            case 'x':
                convert_type = 'x';
                convert_value.u = (uintmax_t) va_arg(argp, unsigned);
                pos += 2;
                break;
            case 'o':
                convert_type = 'o';
                convert_value.u = (uintmax_t) va_arg(argp, unsigned);
                pos += 2;
                break;

            // Int types with size format: long
            case 'l': {
                switch (fmt[pos + 2]) {
                case 'd':
                case 'i':
                    convert_type = 'd';
                    convert_value.s =
                      (intmax_t) va_arg(argp, long);
                    pos += 3;
                    break;
                case 'u':
                    convert_type = 'u';
                    convert_value.u =
                      (uintmax_t) va_arg(argp, unsigned long);
                    pos += 3;
                    break;
                case 'x':
                    convert_type = 'x';
                    convert_value.u =
                      (uintmax_t) va_arg(argp, unsigned long);
                    pos += 3;
                    break;
                case 'o':
                    convert_type = 'o';
                    convert_value.u =
                      (uintmax_t) va_arg(argp, unsigned long);
                    pos += 3;
                    break;
                }
            }

            // Int types with size format: size_t
            case 'z': {
                switch (fmt[pos + 2]) {
                case 'd':
                case 'i':
                    convert_type = 'd';
                    convert_value.s = (intmax_t) va_arg(argp, ssize_t);
                    pos += 3;
                    break;
                case 'u':
                    convert_type = 'u';
                    convert_value.u = (uintmax_t) va_arg(argp, size_t);
                    pos += 3;
                    break;
                case 'x':
                    convert_type = 'x';
                    convert_value.u = (uintmax_t) va_arg(argp, size_t);
                    pos += 3;
                    break;
                case 'o':
                    convert_type = 'o';
                    convert_value.u = (uintmax_t) va_arg(argp, size_t);
                    pos += 3;
                    break;
                }
            }

            }

            // Convert int type to string
            switch (convert_type) {
            case 'd':
                str = buf;
                len = intmax_to_string(convert_value.s, buf, 10);
                handled = true;
                break;
            case 'u':
                str = buf;
                len = uintmax_to_string(convert_value.u, buf, 10);
                handled = true;
                break;
            case 'x':
                str = buf;
                len = uintmax_to_string(convert_value.u, buf, 16);
                handled = true;
                break;
            case 'o':
                str = buf;
                len = uintmax_to_string(convert_value.u, buf, 8);
                handled = true;
                break;
            case 'p':
                str = buf;
                strcpy(buf, "0x");
                len = uintmax_to_string(convert_value.u, buf + 2, 16) + 2;
                handled = true;
                break;
            }
        }

        // Didn't match a format above
        // Handle block of non-format characters
        if (!handled) {
            str = &fmt[pos];
            len = 1 + strcspn(&fmt[pos + 1], "%");
            pos += len;
        }

        // Write output
        if (len > 0) {
            ssize_t ret = rio_writen(fileno, (void *) str, len);
            if (ret < 0 || (size_t) ret != len) {
                return -1;
            }
            num_written += len;
        }
    }

    return num_written;
}

/* Async-signal-safe assertion support*/
void __sio_assert_fail(const char *assertion, const char *file,
                       unsigned int line, const char *function) {
    sio_fprintf(STDERR_FILENO,
                "%s: %s:%u: %s: Assertion `%s' failed.\n",
                __progname, file, line, function, assertion);
    abort();
}


/***************************************************
 * Wrappers for dynamic storage allocation functions
 ***************************************************/

void *Malloc(size_t size) {
    void *p;

    if ((p = malloc(size)) == NULL) {
        perror("Malloc error");
        exit(1);
    }

    return p;
}

void *Realloc(void *ptr, size_t size) {
    void *p;

    if ((p = realloc(ptr, size)) == NULL) {
        perror("Realloc error");
        exit(1);
    }

    return p;
}

void *Calloc(size_t nmemb, size_t size) {
    void *p;

    if ((p = calloc(nmemb, size)) == NULL) {
        perror("Calloc error");
        exit(1);
    }

    return p;
}

void Free(void *ptr) {
    free(ptr);
}


/****************************************
 * The Rio package - Robust I/O functions
 ****************************************/

/*
 * rio_readn - Robustly read n bytes (unbuffered)
 */
ssize_t rio_readn(int fd, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0) {
        if ((nread = read(fd, bufp, nleft)) < 0) {
            if (errno != EINTR) {
                return -1;  /* errno set by read() */
            }

            /* Interrupted by sig handler return, call read() again */
            nread = 0;
        } else if (nread == 0) {
            break;                  /* EOF */
        }
        nleft -= nread;
        bufp += nread;
    }
    return n - nleft;             /* Return >= 0 */
}

/*
 * rio_writen - Robustly write n bytes (unbuffered)
 */
ssize_t rio_writen(int fd, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            printf("nwritten:%d\n",nwritten);
            if (errno != EINTR) {
                return -1;       /* errno set by write() */
            }

            /* Interrupted by sig handler return, call write() again */
            nwritten = 0;
        }
        printf("fd:%d nwritten:%d\n",fd,nwritten);
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}


/*
 * rio_read - This is a wrapper for the Unix read() function that
 *    transfers min(n, rio_cnt) bytes from an internal buffer to a user
 *    buffer, where n is the number of bytes requested by the user and
 *    rio_cnt is the number of unread bytes in the internal buffer. On
 *    entry, rio_read() refills the internal buffer via a call to
 *    read() if the internal buffer is empty.
 */
static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {
    int cnt;

    while (rp->rio_cnt <= 0) {      /* Refill if buf is empty */
        //printf("before read fd:%d\n",rp->rio_fd);
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        //printf("read cnt:%d\n",rp->rio_cnt);
        if (rp->rio_cnt < 0) {
            if (errno != EINTR) {
                return -1;          /* errno set by read() */
            }

            /* Interrupted by sig handler return, nothing to do */
        } else if (rp->rio_cnt == 0) {
            return 0;               /* EOF */
        } else {
            rp->rio_bufptr = rp->rio_buf;   /* Reset buffer ptr */
        }
    }

    /* Copy min(n, rp->rio_cnt) bytes from internal buf to user buf */
    cnt = n;
    if ((size_t) rp->rio_cnt < n) {
        cnt = rp->rio_cnt;
    }
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

/*
 * rio_readinitb - Associate a descriptor with a read buffer and reset buffer
 */
void rio_readinitb(rio_t *rp, int fd) {
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

/*
 * rio_readnb - Robustly read n bytes (buffered)
 */
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nread;
    char *bufp = usrbuf;

    while (nleft > 0) {
        if ((nread = rio_read(rp, bufp, nleft)) < 0) {
            return -1;          /* errno set by read() */
        } else if (nread == 0) {
            break;              /* EOF */
        }
        nleft -= nread;
        bufp += nread;
    }
    return (n - nleft);         /* return >= 0 */
}

/*
 * rio_readlineb - Robustly read a text line (buffered)
 */
ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
    size_t n;
    int rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) {
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n') {
                n++;
                break;
            }
        } else if (rc == 0) {
            if (n == 1) {
                return 0; /* EOF, no data read */
            } else {
                break;    /* EOF, some data was read */
            }
        } else {
            return -1;    /* Error */
        }
    }
    *bufp = 0;
    return n-1;
}

/********************************
 * Client/server helper functions
 ********************************/
/*
 * open_clientfd - Open connection to server at <hostname, port> and
 *     return a socket descriptor ready for reading and writing. This
 *     function is reentrant and protocol-independent.
 *
 *     On error, returns:
 *       -2 for getaddrinfo error
 *       -1 with errno set for other errors.
 */
int open_clientfd(char *hostname, char *port) {
    int clientfd = -1, rc;
    struct addrinfo hints, *listp, *p;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  /* Open a connection */
    hints.ai_flags = AI_NUMERICSERV;  /* ... using a numeric port arg. */
    hints.ai_flags |= AI_ADDRCONFIG;  /* Recommended for connections */
    if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n",
                hostname, port, gai_strerror(rc));
        return -2;
    }
    //printf("p->ai_addr:%s p->ai_addrlen:%d \n",listp->ai_addr->sa_data,listp->ai_addrlen);

    /* Walk the list for one that we can successfully connect to */
    for (p = listp; p; p = p->ai_next) {
        char ipstr[16]; 

    inet_ntop(AF_INET,&(((struct sockaddr_in *)(p->ai_addr))->sin_addr),ipstr, 16);
    printf("ipstr:%s\n", ipstr);
        /* Create a socket descriptor */
        clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (clientfd < 0) {
            continue; /* Socket failed, try the next */
        }

        /* Connect to the server */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) {
            printf("server fd:%d",clientfd);
            break; /* Success */
        }
        printf("connect failed\n");

        /* Connect failed, try another */
        if (close(clientfd) < 0) {
            fprintf(stderr, "open_clientfd: close failed: %s\n",
                    strerror(errno));
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) {   /* All connects failed */
        return -1;
    } else {      /* The last connect succeeded */
        return clientfd;
    }
}

/*
 * open_listenfd - Open and return a listening socket on port. This
 *     function is reentrant and protocol-independent.
 *
 *     On error, returns:
 *       -2 for getaddrinfo error
 *       -1 with errno set for other errors.
 */
int open_listenfd(char *port) {
    struct addrinfo hints, *listp, *p;
    int listenfd = -1, rc, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n",
                port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listenfd < 0) {
            continue;  /* Socket failed, try the next */
        }

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET,
                SO_REUSEADDR, (const void *) &optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
            break; /* Success */
        }

        if (close(listenfd) < 0) { /* Bind failed, try the next */
            fprintf(stderr, "open_listenfd close failed: %s\n",
                    strerror(errno));
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) { /* No address worked */
        return -1;
    }

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
        return -1;
    }
    return listenfd;
}
