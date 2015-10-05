#pragma once
#include <libtorrent/file.hpp>
#include "widget.h"
#include "label.h"
#include "../piece_watcher.h"

#define lt libtorrent

class Remote;

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
    ProgressWatcher(lt::torrent_info *t, lt::storage_interface *st, ProgressBar *pb, Label *l) : PieceWatcher(t, st), bar(pb), info(l) {};
    void setInfoLabel(Label *l);
    void setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset);
    void checkPresence(Remote &r);
private:
    void updateInfo();
    ProgressBar *bar;
    Label *info;    
};
