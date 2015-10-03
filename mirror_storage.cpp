#include <libtorrent/error_code.hpp>
#include "piece_watcher.h"
#include "mirror_storage.h"
#include "helpers.h"

#define ds lt::default_storage
#define pb push_back
#define success(X, Y) if ((X) == -1) { \
                          std::cerr << Y << "\n";}

MirrorStorage::MirrorStorage(lt::storage_params const& params, int fd) :
    default_storage(params), fd(fd) {
    worker = new std::thread(&MirrorStorage::work, this);
}

MirrorStorage::~MirrorStorage() {
    delete worker;
}

void MirrorStorage::initialize(lt::storage_error& err) {
    Query q = Query::initialize;
    success(write_(fd, &q, sizeof(Query)), REMOTE_ERR);
    return ds::initialize(err);
}

int MirrorStorage::writev(lt::file::iovec_t const* buf, int num_bufs, int piece, int offset, int flags, lt::storage_error& err) {
    int code = ds::writev(buf, num_bufs, piece, offset, flags, err);
    if (err.ec == 0) {
	queue.push(WriteRequest(num_bufs, piece, offset, flags, buf[num_bufs - 1].iov_len));
    }
    return code;
}

void MirrorStorage::sendBufs(lt::file::iovec_t const *bufs, int num_bufs) {
    for (int i = 0; i < num_bufs; i++) {
	success(write_(fd, &(bufs[i].iov_len), sizeof(size_t)), REMOTE_ERR);
	success(write_(fd, bufs[i].iov_base, bufs[i].iov_len), REMOTE_ERR);
    }		
}

void MirrorStorage::send(WriteRequest req) {
    SwimmingIOV *bufs = new SwimmingIOV[req.num_bufs];
    bufs[req.num_bufs - 1].iov_len = req.last;
    
    lt::storage_error err;
    ds::readv(bufs, req.num_bufs, req.piece, req.offset, req.flags, err);
    if (err.ec == 0) {
	Query q = Query::writeBuf;
	success(write_(fd, &q, sizeof(Query)), REMOTE_ERR);
	success(write_(fd, &req, sizeof(WriteRequest)), REMOTE_ERR);
	sendBufs(bufs, req.num_bufs);
    }
    delete[] bufs;
}

void MirrorStorage::work() {
    WriteRequest req;
    while (true) {
	queue.pop(req);
	send(req);
    }
}
