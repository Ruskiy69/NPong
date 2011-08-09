/*********************************************************
*                                                        *
*                        SOCKET.H                        *
*                                                        *
*    Created by George Kudrayvtsev (patience).           *
*    Licensed under the Creative Commons License.        *
*                                                        *
*    Version 1.1; Last Modified 8.8.2011                 *
*                                                        *
*    Provides easy socket API access, a simple wrapper   *
*    for both server and client sockets. Somewhat        *
*    modeled after the Python socket module.             *
*                                                        *
*    CLASS METHODS:                                      *
*                                                        *
*    - Socket(family, socket type)                       *
*        If on Windows, initializes the WinSock2 API.    *
*        Sets up the addrinfo struct based on the given  *
*        criteria in the function header.                *
*                                                        *
*    - connect(hostname, port)                           *
*        Connects to the given host on the given port.   *
*                                                        *
*    - sendall(data)    [TCP]                            *
*        Sends the given data to the server.             *
*                                                        *
*    - sendto(hostname, port, data, size of data) [UDP]  *
*        Sends the given data to a server, without       *
*        confirmation.                                   *
*                                                        *
*    - recv_into(buffer, size) [TCP]                     *
*        Receives data from the server, storing it into  *
*        a given buffer, that should be a char array.    *
*                                                        *
*    - recvsome(buffer size) [TCP]                       *
*        Receives a set amount of data, by default 4096  *
*        bytes, and returns a zero-terminated char       *
*        array.                                          *
*                                                        *
*    - recvfrom(hostname, port, buffer, size) [UDP]      *
*        Receives a packet of data and places it into    *
*        a given buffer. Not guaranteed to receive all   *
*        data.                                           *
*                                                        *
*    - recvfrom_some(hostname, port, buffer size) [UDP]  *
*        Receives a packet of data and returns a zero-   *
*        terminated char array filled with received      *
*        information. Default buffer size is 4096 bytes. *
*                                                        *
*    - accept() [SERVER]                                 *
*        Awaits a connection from an incoming client.    *
*                                                        *
*    - bind(hostname, port) [SERVER]                     *
*        Binds the server socket to a given port at the. *
*        given address. Default is localhost:2012.       *
*                                                        *
*    - listen(max waiting connections) [SERVER]          *
*        Sets a maximum amount of connections to pend to *
*        the socket. After the previous connection is    *
*        closed, the server will accept data from the    *
*        next incoming connection. The default is 5.     *
*                                                        *
*    - shutdown(how)                                     *
*        Shuts down the main socket, specifying whether  *
*        it should shut down only sends (SD_SEND [WIN]   *
*        or SHUT_WR [*NIX]), receives (SD_RECV [WIN] or  *
*        SHUT_RD [*NIX]), or both (SD_BOTH [WIN] or      *
*        SHUT_RDWR [*NIX]). The default is shutting down *
*        both sends and receives.                        *
*                                                        *
*    - getSock()                                         *
*        Returns the main socket.                        *
*                                                        *
*********************************************************/

#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>

#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Errors.h"

// Assumes MSVC++ Compiler
#pragma comment(lib, "ws2_32.lib")

#else

#include "Errors.h"
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif // _WIN32

#define LOCALHOST       "127.0.0.1"
#define LOOPBACK        LOCALHOST
#define DEFAULT_PORT    "2012"

// Well known services
#define ECHO            "7"
#define SSH             "22"
#define FTP             "21"
#define SMTP            "25"
#define WHOIS           "43"
#define DNS             "53"
#define HTTP            "80"
#define TOR             "81"
#define SFTP            "115"
#define DCE             "135"
#define SQL             "156"
#define IRC             "194"
#define IMAP            "220"
#define HTTPS           "443"
#define SMB             "445"
#define HTTP_2          "8080"

/* For the NPong (Networked Pong) game */
#define DEF_PONG_HOST   "192.168.1.68"
#define DEF_PONG_PORT   "2012"

enum SOCK_TYPE {TCP_CLIENT, TCP_SERVER, UDP_CLIENT, UDP_SERVER};

class Socket
{
public:

    Socket(const int family, const int sock_type, SOCK_TYPE socket_type = TCP_CLIENT);
    ~Socket();

    /*
        Client-side functions
    */
    void  connect(const char* hostname, const char* port);

    /*
        Server-side functions
    */
    int   accept();
    void  bind(const char* host = LOCALHOST, const char* port = DEFAULT_PORT);
    void  listen(const int backlog = 5);

    /*
        Functions for both server and client sockets
    */
    void  sendall(const char* data);
    void  sendto(const char* hostname, const char* port, const char* data, const int size);

    char* recv(const int bufsize = 4096);
    void  recv_into(char* buffer, const int size);
//    char* recvfrom(const char* hostname, const char* port, const int bufsize = 4096);
//    void  recvfrom_into(const char* hostname, char* buffer, const int size);

    char* nonBlockRecv(const int bufsize = 4096);

    void  setTimeOut(const int timeout);
    int   getTimeOut(){return this->timeout;}

    char* getpeername();
    char* getsockname();

    int   getSock();
    int   getClientSock();

#ifdef _WIN32
    void shutdown(const int how = SD_BOTH);
#else
    void shutdown(const int how = SHUT_RDWR);
#endif // _WIN32

private:
    int                     sock;
    int                     client_sock;

    SOCK_TYPE               socket_type;

    struct addrinfo         hints;
    struct addrinfo*        serv_info;
    struct sockaddr_storage client_addr;
    socklen_t               addr_size;

    int                     timeout;
};

#endif // SOCKET_H