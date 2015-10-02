// handmade rmi for mirrorStorage
// be careful with big/little-endian encodings!
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

class Remote {
public:
    Remote(int fd);
    ~Remote();
    void initialize();
    int listenStorage(lt::default_storage &ds, ProgressWatcher *pw);
private:
    int fd;
    // TODO: queue here
};

// template issue, moving template functions in single translation unit
//#include "remote_interface.cpp"
