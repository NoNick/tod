#include <iostream>
#include "progress.h"

void ProgressBar::draw(unsigned width) {
    std::cout << '[';
    unsigned full = progress * (width - 2) / 100;
    for (unsigned i = 0; i < full; i++) {
	std::cout << '#';
    }
    for (unsigned i = 0; i < width - full - 2; i++) {
	std::cout << ' ';
    }
    std::cout << ']';
}

void ProgressBar::setProgress(unsigned value) {
    progress = value;
}

void ProgressWatcher::setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset) {
    PieceWatcher::setPresent(buf, num_bufs, piece, offset);
    setProgress(getProgress());
}
