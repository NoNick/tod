class Server {
public:
    Server(const char *port);
    int getClient();
    ~Server();
private:
    int sock;
};


