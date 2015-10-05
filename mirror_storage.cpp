#include <libtorrent/error_code.hpp>
#include <unistd.h>
#include "piece_watcher.h"
#include "mirror_storage.h"
#include "helpers.h"

#define ds lt::default_storage
#define pb push_back
#define success(X, Y) if ((X) == -1) { \
                          std::cerr << Y << "\n";}
#define ceilInt(x, y) (x % y ? x / y + 1 : x / y)

MirrorStorage::MirrorStorage(lt::storage_params const& params, int fd,
			     lt::torrent_handle *th, const lt::file_storage *fs) : default_storage(params), fd(fd) {
    sendThread = new std::thread(&MirrorStorage::doSend, this);
    receiveThread = new std::thread(&MirrorStorage::doCheck, this, th, fs);
}

MirrorStorage::~MirrorStorage() {
    delete sendThread;
    delete receiveThread;
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

void MirrorStorage::doCheck(lt::torrent_handle *th, const lt::file_storage *fs) {
    // TODO: is there a better way?
    lt::torrent_status::state_t st = th->status().state;
    while (st == lt::torrent_status::state_t::queued_for_checking ||
	   st == lt::torrent_status::state_t::checking_files ||
	   st == lt::torrent_status::state_t::checking_resume_data) {
	sleep(1);
	st = th->status().state;
    }
    int piece;
    while (true) {
	success(read_(fd, &piece, sizeof(int)), REMOTE_ERR);
	// schedule piece for remote write only if it's downloaded
	// otherwise piece will be sent on write to storage
	if (th->have_piece(piece)) {
	    int last = fs->piece_size(piece) % PieceInfo::BLOCK_SIZE;
	    WriteRequest req(ceilInt(fs->piece_size(piece), PieceInfo::BLOCK_SIZE),
			     piece, 0, 0, last ? last : PieceInfo::BLOCK_SIZE);
	    queue.push(req);
	}
    }
}

void MirrorStorage::doSend() {
    WriteRequest req;
    while (true) {
	queue.pop(req);
	send(req);
    }
}
