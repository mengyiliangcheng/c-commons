/*
 * Starter code for proxy lab.
 * Feel free to modify this code in whatever way you wish.
 */

/* Some useful includes to help you get started */

#include "csapp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <stdbool.h>
#include <inttypes.h>
#include <unistd.h>
#include <assert.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

/*
 * Debug macros, which can be enabled by adding -DDEBUG in the Makefile
 * Use these if you find them useful, or delete them if not
 */
#ifdef DEBUG
#define dbg_assert(...) assert(__VA_ARGS__)
#define dbg_printf(...) fprintf(stderr, __VA_ARGS__)
#else
#define dbg_assert(...)
#define dbg_printf(...)
#endif

/*
 * Max cache and object sizes
 * You might want to move these to the file containing your cache implementation
 */
#define MAX_CACHE_SIZE (1024*1024)
#define MAX_OBJECT_SIZE (100*1024)

/*
 * String to use for the User-Agent header.
 * Don't forget to terminate with \r\n
 */
static const char *header_user_agent = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

static const char *conn_header = "Connection: close\r\n";
static const char *proxy_header = "Proxy-Connection: close\r\n";
static const char *host_header_format = "Host: %s\r\n";
static const char *request_header_format = "GET %s HTTP/1.0\r\n";


/* Typedef for convenience */
typedef struct sockaddr SA;

void serve_main(int connection_fd);

int main(int argc, char **argv) {
    int connfd, listenfd;
    socklen_t addrlen;
    struct sockaddr_in clientaddr;
    char host[MAXLINE], port[MAXLINE];

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    if (listenfd < 0) {
        fprintf(stderr, "Failed to listen on port: %s\n", argv[1]);
        exit(1);
    }

    while (1) {
        /* Initialize the length of the address */
        addrlen = sizeof(clientaddr);

        /* accept() will block until a client connects to the port */
        connfd = accept(listenfd, (SA *) &clientaddr, &addrlen);
        if (connfd < 0) {
            perror("accept");
            continue;
        }

        getnameinfo((SA*) &clientaddr, addrlen, host,
                    MAXLINE, port, MAXLINE, 0);
        printf("Accepted connection from (%s %s).\n", host, port);

        /* Connection is established; serve client */
        serve_main(connfd);
        close(connfd);
    }
}

/*
 * read_requesthdrs - read HTTP request headers
 * Returns true if an error occurred, or false otherwise.
 */
bool read_requesthdrs(rio_t *rp) {
    char buf[MAXLINE];

    do {
        if (rio_readlineb(rp, buf, MAXLINE) <= 0) {
            return true;
        }

        printf("%s", buf);
    } while(strncmp(buf, "\r\n", sizeof("\r\n")));

    return false;
}


/*
 * parse_uri - parse URI into filename and CGI args
 *
 * uri - The buffer containing URI. Must contain a NUL-terminated string.
 * filename - The buffer into which the filename will be placed.
 * cgiargs - The buffer into which the CGI args will be placed.
 * NOTE: All buffers must hold MAXLINE bytes, and will contain NUL-terminated
 * strings after parsing.
 *
 * Returns the appropriate parse result for the type of request.
 */
void parse_uri(char *uri, char *host, char *path, int *port) {
    *port = 80;
    char *web = strstr(uri, "//");

    //Find the uri
    if (web == NULL) {
        web = uri;
    } else {
        web += 2;
    }

    char *pt = strstr(web, ":");
    if (pt != NULL) {
        *pt = '\0';
        sscanf(web, "%s", host);
        sscanf(pt + 1, "%d %s", port, path);
    }
    else {
        pt = strstr(web, "/");

        if (pt != NULL) {
            *pt = '\0';
            sscanf(web, "%s", host);
            *pt = '/';
            sscanf(pt, "%s", path);
        }
        else {
            sscanf(web, "%s", host);
        }
    }

    return;
}


void header(char *web, char *hos, char *path, rio_t *client) {
    char host[MAXLINE] = {0}, request[MAXLINE] = {0}, buf[MAXLINE] = {0}, res[MAXLINE] = {0};

    sprintf(request, request_header_format, path);
    //int n = rio_readlineb(client, buf, MAXLINE);
    while (rio_readlineb(client, buf, MAXLINE) > 0) {
        printf("client header:%s\n",buf);
        if (strcmp(buf, "\r\n") == 0) break;

        if (strncmp(buf, "Host", strlen("Host")) == 0) {
            strcpy(host, buf);
            continue;
        }

        if (!strncasecmp(buf, "Connection", strlen("Connection"))
            &&!strncasecmp(buf, "Proxy-Connection", strlen("Proxy-Connection"))
            &&!strncasecmp(buf, "User-Agent", strlen("User-Agent")))
        {
            strcat(res, buf);
        }
    }

    if(strlen(host) == 0) {
        sprintf(host, host_header_format, hos);
    }

    sprintf(web, "%s%s%s%s%s%s\r\n", request, host, conn_header,
            proxy_header, header_user_agent, res);

    return;
}

/*
 * serve - handle one HTTP request/response transaction
 */
void serve_main(int connection_fd) {
    int port, client_fd;
    rio_t client_rio, server_rio;
    rio_readinitb(&client_rio, connection_fd);

    /* Read request line */
    char buf[MAXLINE];
    if (rio_readlineb(&client_rio, buf, MAXLINE) <= 0) {
        return;
    }

    /* Parse the request line and check if it's well-formed */
    char method[MAXLINE] = {0};
    char uri[MAXLINE] = {0};
    char host[MAXLINE] = {0};
    char path[MAXLINE] = {0};
    char end[MAXLINE] = {0};
    char str[MAXLINE] = {0};
    char version;

    printf("Recived:%s\n",buf);

    /* sscanf must parse exactly 3 things for request line to be well-formed */
    /* version must be either HTTP/1.0 or HTTP/1.1 */
    if (sscanf(buf, "%s %s HTTP/1.%c", method, uri, &version) != 3
            || (version != '0' && version != '1')) {
        printf("Received a malformed request");
        return;
    }
    printf("Recived method:%s\n",method);
    printf("Recived uri:%s\n",uri);
    printf("Recived version:%c\n",version);

    /* Check that the method is GET */
    if (strncmp(method, "GET", sizeof("GET"))) {
        printf("Not implement this method");
        return;
    }

    parse_uri(uri, host, path, &port);
    printf("parse uri:%s host:%s path:%s port:%d\n",uri,host,path,port);
    header(end, host, path, &client_rio);
    //printf("header end:%s host:%s path:%s cline");

    memset(str,0,sizeof(str));
    sprintf(str, "%d", port);
    printf("open client,host:%s,port:%s\n",host,str);
    client_fd = open_clientfd(host, str);
    if (client_fd < 0) {
        printf("Failure\n");
        return;
    }
    printf("client fd:%d\n",client_fd);

    rio_readinitb(&server_rio, client_fd);
    int ret = rio_writen(client_fd, end, strlen(end));
    printf("writen:%d end:[%s]\n",ret,end);
    int n;
    while((n = rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
        printf("Received %d bytes\n", n);
        printf("Received %s\n",buf);
        rio_writen(connection_fd, buf, n);
    }
    close(client_fd);
}
