#include <stdlib.h>
#include "iov_factory.h"
#include "piece_watcher.h"

iov* IOVFactory::alloc(int num_bufs) {
    iov *res = new iov[num_bufs];
    for (int i = 0; i < num_bufs; i++) {
	res[i].iov_base = pool::malloc();
	res[i].iov_len = PieceInfo::BLOCK_SIZE;
    }
    return res;
}

void IOVFactory::dealloc(iov *bufs, int num) {
    for (iov* buf = bufs; buf != bufs + num; buf++) {
	pool::free(buf->iov_base);
    }
    delete[] bufs;
}
