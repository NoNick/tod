#pragma once
#include <libtorrent/file.hpp>
#include "widget.h"
#include "label.h"
#include "../piece_watcher.h"

// draws progress bar, progress varies from 0 to 100
class ProgressBar : public Widget {
public:
    ProgressBar(Screen *s) : Widget(s), progress(0) {};
    void draw(unsigned width);
    void setProgress(unsigned value);
private:
    unsigned progress;
};

// watch pieces and display progress
class ProgressWatcher : public PieceWatcher {
public:
    ProgressWatcher(const lt::file_storage &fs, ProgressBar *pb, Label *l) : PieceWatcher(fs), bar(pb), info(l) {};
    void setInfoLabel(Label *l);
    void setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset);
private:
    ProgressBar *bar;
    Label *info;    
};
