#pragma once
#include <thread>
#include <tbb/concurrent_queue.h>
#include <libtorrent/storage.hpp>
#include <libtorrent/storage_defs.hpp>
#include "remote_interface.h"
#include "swimming_iov.h"

#define lt libtorrent

// works as default_storage, but forwards mathods calls to given fd
// server part of remote interface
class MirrorStorage : public lt::default_storage {
public:
    MirrorStorage(lt::storage_params const& params, int fd);
    virtual ~MirrorStorage();

    void initialize(lt::storage_error& err);
    int writev(lt::file::iovec_t const*, int, int, int, int, lt::storage_error& err);
    void work();

private:
    std::thread *worker;
    tbb::concurrent_bounded_queue<WriteRequest> queue;
    int fd;
    void write(lt::file::iovec_t const *bufs, int num_bufs, int piece, int offset, int flags);
    void send(WriteRequest req);
    void sendBufs(lt::file::iovec_t const *bufs, int num_bufs);
};
