#include <iostream>
#include <string>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <boost/program_options.hpp>
#include "helpers.h"

#define critical(X, Y) if ((X) == -1) {std::cerr << Y << "\n"; _exit(3);}

namespace po = boost::program_options;

using namespace std;

const int BUF_SIZE = 4096;

int getSocket(string addr, int port) {
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent *server = gethostbyname(addr.c_str());
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr,
	   server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    critical(connect(s, (struct sockaddr*) & serv_addr, sizeof(serv_addr)), "Connection failed");

    return s;
}

void sendFile(int sock, string file) {
    struct stat stat_buf;
    int rc = stat(file.c_str(), &stat_buf);
    int sz = rc == 0 ? stat_buf.st_size : -1;

    write_(sock, &sz, 4);

    int f;
    critical(f = open(file.c_str(), O_RDONLY), "No such file");
    buf_t* buf = buf_new(BUF_SIZE);
    int n = 0;
    while (n != -1 && (n = buf_fill(f, buf, BUF_SIZE)) > 0) {
        n = buf_flush(sock, buf, n);
    }
    buf_free(buf);
}

int main (int ac, char *av[]) {
        po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce help message")
	("addr", po::value<string>(), "server address")
	("port", po::value<int>(), "port for daemon (default: 1234)")
	("file", po::value<string>(), "torrent file to be used");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
	cout << desc << "\n";
	return 1;
    } else if (!vm.count("file")) {
	cout << "Please specify torrent file\n";
        return 2;
    } else if (!vm.count("addr")) {
	cout << "Please specify server address\n";
	return 3;
    }

    string file = vm["file"].as<string>();
    int port = vm.count("port") ? vm["port"].as<int>() : 1234;
    string addr = vm["addr"].as<string>();

    int sock;
    critical(sock = getSocket(addr, port), "Unable to connect");
    critical(handshake(sock), SHAKE_ERR);
    sendFile(sock, file);
    close(sock);

    return 0;
}
