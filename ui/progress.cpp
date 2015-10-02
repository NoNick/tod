#include <iostream>
#include "progress.h"

#define STR(X) std::to_string(X)

std::string bpsToString(unsigned bps) {
    if (bps > 10 * 1024) {
	if (bps > 10 * 1024 * 1024) {
	    return STR(bps / 1024 / 1024) + " MBps";
	}
	return STR(bps / 1024) + " KBps";
    }
    return STR(bps) + " Bps";
}

std::string secToString(unsigned sec) {
    if (sec > 60) {
	if (sec > 60 * 60) {
	    if (sec > 60 * 60 * 24) {
		return STR(sec / 60 / 60 / 24) + " days, " +
		    STR((sec / 60 / 60) % 24) + " hrs";
	    }
	    return STR(sec / 60 / 60) + " hrs, " +
		STR((sec / 60) % 60) + " min";
	}
	return STR(sec / 60) + " min, " + STR(sec % 60) + " sec";
    }
    return STR(sec) + " sec";
}

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
    screen->refresh();
}

void ProgressWatcher::setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset) {
    PieceWatcher::setPresent(buf, num_bufs, piece, offset);
    unsigned p = getProgress();
    bar->setProgress(p);
    unsigned s = getSpeed();
    if (s == 0) {
	return;
    }
    info->setText(bpsToString(s) + " EST: " +
		  secToString(getEstimated()) + " " + STR(p) + "% done");
}

void ProgressWatcher::setInfoLabel(Label *l) {
    info = l;
}
