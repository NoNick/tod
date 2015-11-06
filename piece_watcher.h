#pragma once
#include <boost/chrono.hpp>
#include <libtorrent/file.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/storage.hpp>
#include "ui/text_area.h"

#define lt libtorrent
#define bc boost::chrono
#define NOW bc::time_point_cast<mcs>(bc::steady_clock::now())

typedef bc::duration<unsigned long, boost::micro> mcs;
typedef bc::time_point<bc::steady_clock, mcs> moment;

class Remote;

struct PieceInfo {
    // mask of downloaded (1) and not present (0) blocks
    std::vector <bool> present;
    // total size of piece in blocks
    unsigned size:16;
    // number of not present blocks
    unsigned remain:15;
    // true if reman == 0
    bool finished:1;

    // size in bytes
    void setSize(unsigned size);
    // marks [start, start + len) blocks as present
    // returns number of blocks which weren't present
    unsigned setPresent(unsigned start, unsigned len);

    // assume piece has 16 KB blocks (in case of 32 KB blocks let's mark two blocks instead of one)
    static const unsigned BLOCK_SIZE = 16 * 1024;
};

// tracks downloaded pieces and blocks
// maintain weighted average speed of download
class PieceWatcher {
public:
    PieceWatcher(lt::torrent_info *t, lt::storage_interface *st);
    virtual ~PieceWatcher();
    void setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset);
    // percentage of present blocks
    unsigned getProgress();
    // weigthed average speed, bytes per second
    unsigned getSpeed();
    // time in seconds based on average speed
    unsigned getEstimated();
    // iterates through unpresented pieces and set them presented
    // if storage contains this piece and it has correct hash
    // for unpresent pieces send request for then to server
    void checkPresence(Remote &r);
    bool finished();
private:
    // returns true if hash of piece in storage is correct
    bool correctPiece(int piece);
    // true if piece is done
    bool setPresent_(unsigned sz, int piece, int offset);
    
    // piece length (except for the last one)
    unsigned len;
    // number of pieces/blocks
    unsigned pNum, bNum;
    // number of blocks 
    unsigned pRemain = 0, bRemain = 0;
    PieceInfo *p;
    lt::torrent_info *torrent;
    lt::storage_interface *st;

    // let's update speed every INTERVAL mcs
    const unsigned long INTERVAL = 1000000;
    // consider last NUM measures
    const unsigned NUM = 10;
    unsigned long B = 0;
    mcs T;
    // last update
    moment t;
    unsigned speed = 0;
    // downloaded since last update
    void updateSpeed(unsigned bytes);
    // downloaded during last INTERVAL mcs
    void makeMeasure(unsigned bytes);
    bool invalidTime;
};
