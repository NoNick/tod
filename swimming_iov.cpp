#include <stdlib.h>
#include "swimming_iov.h"
#include "piece_watcher.h"

SwimmingIOV::SwimmingIOV() {
    iov_base = pool::malloc();
    iov_len = PieceInfo::BLOCK_SIZE;
}

SwimmingIOV::~SwimmingIOV() {
    pool::free(iov_base);
}
