#pragma once

// listens for connections on given port
class Server {
public:
    Server(const char *port);
    // accept next connection in queue
    int getClient();
    // close socket
    ~Server();
private:
    int sock;
};
