#include <iostream>
#include <exception>
#include <string>
#include <errno.h>
#include <libtorrent/storage.hpp>
#include "helpers.h"
#include "remote_interface.h"
#include "iov_factory.h"
#include "ui/text_area.h"

#define success(X, Y) if ((X) == -1) { \
	                  ERR(Y); \
                      }

Remote::Remote(int fd) : fd(fd) {}

Remote::~Remote() { close(fd); }

void receiveVec(int fd, lt::file::iovec_t *bufs, int num_bufs) {
    for (int i = 0; i < num_bufs; i++) {
	size_t sz = 0;
	success(read_(fd, &sz, sizeof(size_t)), REMOTE_ERR);
	success(read_(fd, bufs[i].iov_base, sz), REMOTE_ERR);
	bufs[i].iov_len = sz;
    }
}

void Remote::requestPiece(int piece) {
    write_(fd, &piece, sizeof(int));
}

// returns 0 for success
//         1 for network error
//         2 for storage error
int Remote::listenStorage(lt::default_storage &def, ProgressWatcher *pw) {
    Query q = (Query)0;
    success(read_(fd, &q, sizeof(Query)), REMOTE_ERR);

    switch(q) {
    case Query::initialize: {
	lt::storage_error err;
	def.initialize(err);
	break; }
    case Query::writeBuf: {
	lt::storage_error err;
	WriteRequest req(0, 0, 0, 0, 0);
	success(read_(fd, &req, sizeof(WriteRequest)), REMOTE_ERR);
	lt::file::iovec_t *bufs = IOVFactory::alloc(req.num_bufs);
	try {
	    receiveVec(fd, bufs, req.num_bufs);
	} catch (std::exception e) {
	    delete[] bufs;
	    std::cout << "bufs exception\n";
	    _exit(1);
	}
	def.writev(bufs, req.num_bufs, req.piece, req.offset, req.flags, err);
	pw->setPresent(bufs, req.num_bufs, req.piece, req.offset);
	IOVFactory::dealloc(bufs, req.num_bufs);
	break; }
    }
	  
    return 0;
}
