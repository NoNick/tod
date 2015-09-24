#pragma once
#include <libtorrent/file.hpp>
#include "widget.h"
#include "../piece_watcher.h"

namespace lt = libtorrent;

// draws progress bar, progress varies from 0 to 100
class ProgressBar : public Widget {
public:
    ProgressBar() : progress(0) {};
    void draw(unsigned width);
    void setProgress(unsigned value);
private:
    unsigned progress;
};

class ProgressWatcher : public ProgressBar, public PieceWatcher {
public:
    ProgressWatcher(const lt::file_storage &fs) : PieceWatcher(fs) {};
    void setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset);
};
