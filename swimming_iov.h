#pragma once
#include <boost/pool/singleton_pool.hpp>
#include <libtorrent/file.hpp>
#include "piece_watcher.h"

struct IOV {};

typedef boost::singleton_pool<IOV, PieceInfo::BLOCK_SIZE> pool;

// same as iovec_t, but in constructor allocates BLOCK_SIZE base
// using boost singleton pool
// in destructor frees base
struct SwimmingIOV : libtorrent::file::iovec_t {
    SwimmingIOV();
    virtual ~SwimmingIOV();
};
