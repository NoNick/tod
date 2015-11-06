#pragma once
#include <thread>
#include <tbb/concurrent_queue.h>
#include <libtorrent/storage.hpp>
#include <libtorrent/storage_defs.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include "remote_interface.h"
#include "iov_factory.h"

#define lt libtorrent

// works as default_storage, but forwards mathods calls to given fd
// server part of communication subsystem
class MirrorStorage : public lt::default_storage {
public:
    MirrorStorage(lt::storage_params const& params, int fd, lt::torrent_handle *th, const lt::file_storage *fs);
    virtual ~MirrorStorage();

    void initialize(lt::storage_error& err);
    int writev(lt::file::iovec_t const*, int, int, int, int, lt::storage_error& err);

private:
    // following two meant to be run in dedicated threads
    // loops sending writes from queue 
    void doSend();
    // loops receiving requests and scheduling writes
    void doCheck(lt::torrent_handle *th, const lt::file_storage *fs);
	
    std::thread *sendThread, *receiveThread;
    tbb::concurrent_bounded_queue<WriteRequest> queue;
    int fd;
    void send(WriteRequest req);
    void sendBufs(lt::file::iovec_t const *bufs, int num_bufs);
};
