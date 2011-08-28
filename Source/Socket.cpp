#include "Socket.h"

Socket::Socket(const int family, const int sock_type, SOCK_TYPE socket_type)
{
#ifdef _WIN32
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
        handleError("Unable to initialize WinSock!");
#endif // _WIN32

    this->sock          = 0;
    this->client_sock   = 0;
    this->socket_type   = socket_type;
    this->timeout       = 2;

    memset(&this->hints, 0, sizeof this->hints);

    this->hints.ai_family   = family;
    this->hints.ai_socktype = sock_type;
}

Socket::Socket()
{
#ifdef _WIN32
    WSADATA wsadata;
    if(WSAStartup(MAKEWORD(2, 0), &wsadata) != 0)
        handleError("Unable to initialize WinSock!");
#endif // _WIN32

    this->sock          = INVALID_SOCKET;
    this->client_sock   = INVALID_SOCKET;
    this->timeout       = 10;

    memset(&this->hints, 0, sizeof this->hints);
}

Socket::~Socket()
{
#ifdef _WIN32
    WSACleanup();
    closesocket(this->sock);
    closesocket(this->client_sock);
#else
    close(this->sock);
    close(this->client_sock);
#endif // _WIN32
}

void Socket::connect(const char* hostname, const char* port)
{
    if(this->socket_type == TCP_SERVER || this->socket_type == UDP_SERVER)
    {
        handleError("Cannot use connect() for a server socket!");
    }

    struct addrinfo* tmp;

    int status = getaddrinfo(hostname, port, &this->hints, &tmp);

    if(status != 0)
    {
        handleError("Failed to get address information.\nCheck your internet connection and try again!");
    }

    for(this->serv_info = tmp; this->serv_info != NULL; this->serv_info = this->serv_info->ai_next)
    {
        this->sock = socket(this->serv_info->ai_family, this->serv_info->ai_socktype, this->serv_info->ai_protocol);
        if(this->sock == -1)
        {
            continue;
        }

        if(::connect(this->sock, this->serv_info->ai_addr, this->serv_info->ai_addrlen) == -1)
        {
#ifdef _WIN32
            closesocket(this->sock);
#else
            close(this->sock);
#endif // _WIN32
            continue;
        }
        break;
    }

    if(this->serv_info == NULL)
    {
        char* error = new char[256];

#ifdef _WIN32
        sprintf_s(error, 256, "Unable to connect to %s", hostname);
#else
        sprintf(error, "Unable to connect to %s", hostname);
#endif // _WIN32

        handleError(error);
        delete[] error;
    }

    freeaddrinfo(tmp);
}

int Socket::accept()
{
    this->addr_size     = sizeof this->client_addr;
    this->client_sock   = ::accept(this->sock, (struct sockaddr*) &this->client_addr, &this->addr_size);
    return this->client_sock;
}

int Socket::nonBlockAccept()
{
    static bool setup = false;

    /* This is only done on the first call to nonBlockAccept() */
    if(!setup)
    {
        u_long no_block = 1;
        if(ioctlsocket(this->sock, FIONBIO, &no_block) != NO_ERROR)
        {
            handleError("Unable to make a non-blocking socket!");
        }
        this->addr_size = sizeof this->client_addr;
    }

    setup               = true;
    int tmp             = ::accept(this->sock, (struct sockaddr*) &this->client_addr, &this->addr_size);

    if(tmp != -1)
        this->client_sock = tmp;

    return tmp;
}

void Socket::bind(const char* host, const char* port)
{
    if(this->socket_type != TCP_SERVER && this->socket_type != TCP_CLIENT)
    {
        handleError("Bind() only works for server sockets!");
    }

    if(host != NULL)
        this->hints.ai_flags    = AI_PASSIVE;

    struct addrinfo* tmp;

    int status = getaddrinfo(NULL, port, &this->hints, &tmp);

    if(status != 0)
    {
        handleError("Failed to get address information.\nCheck your internet connection and try again!");
    }

    for(this->serv_info = tmp; this->serv_info != NULL; this->serv_info = this->serv_info->ai_next)
    {
        this->sock = socket(this->serv_info->ai_family, this->serv_info->ai_socktype, this->serv_info->ai_protocol);
        if(this->sock == -1)
        {
            continue;
        }

        if(::bind(this->sock, tmp->ai_addr, tmp->ai_addrlen) == -1)
        {
#ifdef _WIN32
            closesocket(this->sock);
#else
            close(this->sock);
#endif // _WIN32
            continue;
        }
        break;
    }

    if(this->serv_info == NULL)
    {
        char* error = new char[256];

#ifdef _WIN32
        sprintf_s(error, 256, "Unable to bind to port %s", port);
#else
        sprintf(error, "Unable to bind to port %s", port);
#endif // _WIN32

        handleError(error);
        delete[] error;
    }

    freeaddrinfo(tmp);
}

void Socket::listen(const int backlog)
{
    ::listen(this->sock, backlog);
}

int Socket::ping(const char* host, const int ttl_tmp)
{
    int seq_no              = 0;
    int packet_size         = DEFAULT_PACKET_SIZE;
    int ttl                 = ttl_tmp;
    int retval              = 0;

    ICMPHeader* send_buf    = NULL;
    IPHeader* recv_buf      = NULL;
    int sd                  = INVALID_SOCKET;

    sockaddr_in dest, source;
    
    packet_size = max(sizeof(ICMPHeader), min(MAX_PING_DATA_SIZE, (unsigned int)packet_size));

    send_buf = (ICMPHeader*)new char[packet_size];  
    recv_buf = (IPHeader*)new char[MAX_PING_PACKET_SIZE];
    if(send_buf == NULL || recv_buf == NULL)
        handleError("Not enough memory!");

    retval = this->ICMP_Setup(host, ttl, sd, dest);
    if(retval == -1)
        return retval;
    
    this->ICMP_Init(send_buf, packet_size, seq_no);

    retval = this->ICMP_Send(sd, dest, send_buf, packet_size);

    if(retval >= 0)
    {
        while(true)
        {
            retval = this->ICMP_Recv(sd, source, recv_buf, (const int)MAX_PING_PACKET_SIZE);
            if(retval < 0)
                break;

            retval = this->ICMP_Decode(recv_buf, packet_size, &source);
            if(retval != -2)
                break;
        }
    }

    delete[] send_buf;
    delete[] recv_buf;

    return retval;
}

void Socket::setNonBlocking()
{
#ifdef _WIN32
    unsigned long on = 1;
    ioctlsocket(this->sock, FIONBIO, &on);
#else
    fcntl(this->sock, F_SETFL, O_NONBLOCK);
#endif // _WIN32
}

void Socket::sendall(const char* data)
{
    int to_send = strlen(data);
    int sent    = 0;
    int total   = 0;

    while(total < to_send)
    {
        if(this->socket_type == TCP_CLIENT || this->socket_type == UDP_CLIENT)
            sent = send(this->sock, data + sent, to_send, NULL);
        else
        {
            if(this->client_sock == INVALID_SOCKET)
                handleError("Not connected to a client!", false);
            else
                sent = send(this->client_sock, data + sent, to_send, NULL);
        }

        if(sent == 0)
            break;
        else if(sent == SOCKET_ERROR)
        {
            handleError("Unable to send request!");
        }
        total   += sent;
        to_send -= sent;
    }
}

void Socket::sendto(const char* hostname, const char* port, const char* data, const int size)
{
    ::sendto(this->sock, data, size, NULL, this->serv_info->ai_addr, this->serv_info->ai_addrlen);
}

char* Socket::recv(const int bufsize)
{
    char* buffer = new char[bufsize];
    int recvd    = 0;

    if(this->socket_type == TCP_CLIENT)
        recvd = ::recv(this->sock, buffer, bufsize, NULL);
    else if(this->socket_type == UDP_CLIENT)
        handleError("Use recvfrom() for UDP sockets!");
    else
        recvd = ::recv(this->client_sock, buffer, bufsize, NULL);

    if(recvd == 0)
        return NULL;

    buffer[recvd] = 0;

    return buffer;
}

void Socket::recv_into(char* buffer, const int size)
{
    int bytes_recvd = 0;

    if(this->socket_type == TCP_CLIENT || this->socket_type == UDP_CLIENT)
        bytes_recvd = ::recv(this->sock, buffer, size, NULL);
    else
    {
        if(this->client_sock == 0)
            handleError("No client connected!");
        else
            bytes_recvd = ::recv(this->client_sock, buffer, size, NULL);
    }

    buffer[bytes_recvd] = 0;
}

char* Socket::nonBlockRecv(const int bufsize)
{
    fd_set            recv_fds;
    struct timeval    timeout;
    int                retval;

    FD_ZERO(&recv_fds);
    FD_SET(this->getSock(), &recv_fds);

    timeout.tv_sec  = this->timeout;
    timeout.tv_usec = 0;

    retval = select(this->getSock() + 1, &recv_fds, NULL, NULL, &timeout);

    if(retval == -1 || retval == 0)
        return NULL;

    else
        return this->recv(bufsize);
}

void Socket::setTimeOut(const int timeout)
{
    this->timeout = timeout;
}

char* Socket::getpeername()
{
    int port = 0;
    char hostname[INET_ADDRSTRLEN];
    socklen_t name_size = sizeof this->client_addr;
    ::getpeername(this->sock, (struct sockaddr*)&this->client_addr, &name_size);
    struct sockaddr_in* addr = (sockaddr_in*)&this->client_addr;
    port = ntohs(addr->sin_port);
    inet_ntop(AF_INET, &addr->sin_addr, hostname, sizeof hostname);
    char* peername = new char[INET_ADDRSTRLEN + 5];

#ifdef _WIN32
    sprintf_s(peername, strlen(peername), "%s:%d", hostname, port);
#else
    sprintf(peername, "%s:%d", hostname, port);
#endif // _WIN32

    return peername;
}

void Socket::shutdown(const int how)
{
    if(this->client_sock != 0)
        ::shutdown(this->client_sock, how);

    ::shutdown(this->sock, how);
}

int Socket::getSock()
{
    return this->sock;
}

int Socket::getClientSock()
{
    if(this->socket_type == TCP_SERVER || this->socket_type == UDP_SERVER)
        return this->client_sock;
    else
        return -1;
}

int Socket::ICMP_Setup(const char* host, const int ttl, int& sd, sockaddr_in& dest)
{
    sd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    
    if(sd == INVALID_SOCKET)
        return -1;

    setsockopt(sd, IPPROTO_IP, IP_TTL, (const char*) &ttl, sizeof ttl);

    memset(&dest, 0, sizeof dest);

    UINT addr = inet_addr(host);
    dest.sin_addr.s_addr = addr;
    dest.sin_family = AF_INET;

    hostent* hp = gethostbyname(host);
    memcpy(&(dest.sin_addr), hp->h_addr, hp->h_length);
    dest.sin_family = hp->h_addrtype;

    return 0;
}

void Socket::ICMP_Init(ICMPHeader* icmp, const int size, const int seq_no)
{
    icmp->type      = ICMP_ECHO_REQUEST;
    icmp->code      = 0;
    icmp->chk       = 0;
    icmp->id        = (unsigned short)GetCurrentProcessId();
    icmp->seq       = seq_no;
    icmp->time      = GetTickCount();

    const unsigned long int dead = 0xDEADBEEF;
    char* datapart = (char*)icmp + sizeof(ICMPHeader);
    int to_send = size - sizeof(ICMPHeader);

    while(to_send > 0)
    {
        memcpy(datapart, &dead, min(int(sizeof dead), to_send));
        to_send -= sizeof dead;
        datapart += sizeof dead;
    }

    icmp->chk = this->ICMP_Checksum((unsigned short*)icmp, size);
}

int Socket::ICMP_Send(int sd, sockaddr_in& dest, ICMPHeader* send_buf, const int size)
{
    int sent = ::sendto(sd, (char*)send_buf, size, 0, (sockaddr*)&dest, sizeof dest);

    if(sent == SOCKET_ERROR)
        return -1;

    return 0;
}

int Socket::ICMP_Recv(int sd, sockaddr_in& src, IPHeader* recv_buf, const int size)
{
    int fromlen = sizeof src;
    int bread   = recvfrom(sd, (char*)recv_buf, size + sizeof(IPHeader), 0, (sockaddr*)&src, &fromlen);
    if(bread == -1)
    {
#ifdef _WIN32
        if(WSAGetLastError() == WSAEWOULDBLOCK)
            return 0;
        else
            return -1;
#else
        if(errno == EWOULDBLOCK)
            return 0;
        else
            return -1;
#endif // _WIN32
    }
    else
        return 0;
}

int Socket::ICMP_Decode(IPHeader* reply, const int bytes, sockaddr_in* from)
{
    unsigned short header_len = reply->h_len * 4;
    ICMPHeader* icmp = (ICMPHeader*)((char*)reply + header_len);

    if(bytes < header_len + ICMP_PACKET_SIZE)
    {
        return -1;
    }
    else if(icmp->type != ICMP_ECHO_REPLY)
    {
        if(icmp->type != ICMP_TTL_EXPIRE)
        {
            return -1;
        }
    }
    else if(icmp->id != (unsigned short)GetCurrentProcessId())
        return -2;

    return 0;
}

unsigned short Socket::ICMP_Checksum(unsigned short* buffer, int size) 
{
    unsigned long cksum = 0;
    
    // Sum all the words together, adding the final byte if size is odd
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(unsigned short);
    }
    if (size) {
        cksum += *(UCHAR*)buffer;
    }

    // Do a little shuffling
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    
    // Return the bitwise complement of the resulting mishmash
    return (unsigned short)(~cksum);
}
