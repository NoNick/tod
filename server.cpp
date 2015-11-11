#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include "server.h"
#include "txt.cpp"

#define run(X) if ((X) == -1) return -1;

void setHints(struct addrinfo *info) {
    memset(info, 0, sizeof(struct addrinfo));
    info->ai_family = AF_INET;
    info->ai_socktype = SOCK_STREAM;
    info->ai_protocol = IPPROTO_TCP;
    info->ai_flags = AI_PASSIVE;
    info->ai_canonname = NULL;
    info->ai_addr = NULL;
    info->ai_next = NULL;
}

/*
 * Finds address which can be binded to socket so it can be listened on.
 * Returns new socket's descriptor or -1 if failed
 */
int setUpSocket(struct addrinfo *addrs) {
    int sfd;
    struct addrinfo *ptr;
    for (ptr = addrs; ptr != NULL; ptr = ptr->ai_next) {
        sfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sfd == -1) {
            continue;
	}

	int enable = 1;
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
	    close(sfd);
	    continue;
	}
	
        if (bind(sfd, ptr->ai_addr, ptr->ai_addrlen) == -1) {
	    std::cerr << strerror(errno);
	    close(sfd);
	    continue;
	}

	if (listen(sfd, SOMAXCONN) == -1) {
	    close(sfd);
	} else {
	    return sfd;
	}
    }

    return -1;
}

int getServSocket(const char *port) {
    struct addrinfo hints, *localhosts;
    int serverSocket;
    setHints(&hints);
    run(getaddrinfo(NULL, port, &hints, &localhosts));
    run(serverSocket = setUpSocket(localhosts));
    freeaddrinfo(localhosts);
    return serverSocket;
}

Server::Server(const char *port) {
    sock = getServSocket(port);
    if (sock == -1) {
	throw TxtException("Unable to listen\n");
    }
}

int Server::getClient() {
    struct sockaddr_in client;
    socklen_t sz = sizeof(client);
    return accept(sock, (struct sockaddr*)&client, &sz);
}

Server::~Server() {
    close(sock);
}
