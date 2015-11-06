#include <iostream>
#include <string>
#include <exception>
#include <boost/program_options.hpp>
#include "server.h"
#include "txt.cpp"
#include "obtainer.h"

namespace po = boost::program_options;

using namespace std;

bool verbose;
string storage;
string port;

int main(int ac, char *av[]) {
    po::options_description desc("Allowed options");
    desc.add_options()
	("help", "produce help message")
	("verbose", "set verbose mode")
	("storage", po::value<string>(), "set dir to store downloads (default: .)")
	("port", po::value<string>(), "port for daemon (default: 1234)");

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    
    if (vm.count("help")) {
	cout << desc << "\n";
	return 1;
    }
    verbose = vm.count("verbose");
    storage = vm.count("storage") ? vm["storage"].as<string>() : ".";
    port = vm.count("port") ? vm["port"].as<string>() : "1234";

    Server *serv;
    try {
	serv = new Server(port.c_str());
    } catch (exception e) {
	cout << e.what() << "\n";
	return 2;
    }
    
    if (verbose) {
	cout << "Ready to accept connections\nPort: " + port + "\n";
    }
    int fd;
    while (true) {
	fd = serv->getClient();
	if (fd == -1) {
	    cerr << "Failed to accept connection\n";
	} else if (fork() == 0) {
	    Obtainer ob(fd);
	    ob.run();
	    _exit(0);
	}
	close(fd);
    }

    delete serv;
    return 0;
}
