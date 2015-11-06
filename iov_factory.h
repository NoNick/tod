#pragma once
#include <boost/pool/singleton_pool.hpp>
#include <libtorrent/file.hpp>
#include "piece_watcher.h"

typedef libtorrent::file::iovec_t iov;

struct IOV {};

typedef boost::singleton_pool<IOV, PieceInfo::BLOCK_SIZE> pool;

// allocates and deallocates buffers of BLOCK_SIZE using boost pool
// note: can't inhert new iov struct from iovec_t and overload constructor:
//       in such case sizeof(newIOV) != sizeof(iovec_t) because of vptr
//       so operator[] would work incorrect with newIOV treated as iovec_t
class IOVFactory {
public:
    static iov *alloc(int num_bufs);
    static void dealloc(iov *bufs, int num);
private:
    IOVFactory();
    virtual ~IOVFactory();
};
