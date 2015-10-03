#include <iostream>
#include <exception>
#include <string>
#include <errno.h>
#include <libtorrent/storage.hpp>
#include "helpers.h"
#include "remote_interface.h"
#include "swimming_iov.h"

#define success(X, Y) if ((X) == -1) { \
                          std::cerr << Y << "\n"; \
                          return -1; }
#define successE(X) if ((X) == -1) throw std::exception();

Remote::Remote(int fd) : fd(fd) {}

Remote::~Remote() { close(fd); }

void receiveVec(int fd, iov *bufs, int num_bufs) {
    for (int i = 0; i < num_bufs; i++) {
	size_t sz = 0;
	successE(read_(fd, &sz, sizeof(size_t)));
	successE(read_(fd, bufs[i].iov_base, sz));
	bufs[i].iov_len = sz;
    }
}

// returns 0 for success
//         1 for network error
//         2 for storage error
int Remote::listenStorage(lt::default_storage &def, ProgressWatcher *pw) {
    Query q = (Query)0;
    read_(fd, &q, sizeof(Query));
    //std::cout << f << "\n";

    switch(q) {
    case Query::initialize: {
	//std::cout << "initializing...\n";
	lt::storage_error err;
	def.initialize(err);
	//result;
	break; }
    case Query::writeBuf: {
	lt::storage_error err;
	WriteRequest req(0, 0, 0, 0, 0);
	success(read_(fd, &req, sizeof(WriteRequest)), REMOTE_ERR);
	SwimmingIOV *bufs = new SwimmingIOV[req.num_bufs];
	try {
	    receiveVec(fd, bufs, req.num_bufs);
	} catch (std::exception e) {
	    std::cout << "bufs exception\n";
	    _exit(1);
	}
	//	do {
	def.writev(bufs, req.num_bufs, req.piece, req.offset, req.flags, err);
	//std::cout << "piece #" << *piece << ": " << def.writev(bufs, *num_bufs, *piece, *offset, *flags, err) << " bytes are written\n";
	    //} while (err.operation != lt::storage_error::file_operation_t::none);
	pw->setPresent(bufs, req.num_bufs, req.piece, req.offset);
	//result;
	delete[] bufs;
	break; }
    }
	  
    return 0;
}
