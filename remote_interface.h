#pragma once
#include <stdarg.h>
#include <functional>
#include <tbb/concurrent_queue.h>
#include <libtorrent/storage.hpp>
#include "ui/progress.h"

#define lt libtorrent

const char * const REMOTE_ERR = "Connection problem, aborting query.";

enum Query {
    writeBuf, initialize
};

struct WriteRequest {
    // last block can be different from 16KB size
    int num_bufs, piece, offset, flags, last;

    WriteRequest(int n, int p, int o, int f, int l) :
      num_bufs(n), piece(p), offset(o), flags(f), last(l) {};
    WriteRequest() {};
};

// client part for communication subsystem
class Remote {
public:
    // uses fd descriptor for communication
    Remote(int fd);
    ~Remote();
    // listens for incoming storage operations
    // does them in ds, notifies pw
    int listenStorage(lt::default_storage &ds, ProgressWatcher *pw);
    // send server request to schedule remote write of piece
    // need for re-download of corrupted pieces and resume function
    void requestPiece(int piece);
private:
    int fd;
};

