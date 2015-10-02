#pragma once
#include <boost/chrono.hpp>
#include <libtorrent/file.hpp>
#include <libtorrent/file_storage.hpp>
#include "ui/text_area.h"

#define lt libtorrent
#define bc boost::chrono
#define NOW bc::time_point_cast<mcs>(bc::steady_clock::now())

typedef bc::duration<unsigned long, boost::micro> mcs;
typedef bc::time_point<bc::steady_clock, mcs> moment;

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
    PieceWatcher(const lt::file_storage &fs);
    virtual ~PieceWatcher();
    void setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset);
    // percentage of present blocks
    unsigned getProgress();
    // weigthed average speed, bytes per second
    unsigned getSpeed();
    // time in seconds based on average speed
    unsigned getEstimated();
    bool finished();
private:
    // piece length (except for the last one)
    unsigned len;
    // number of pieces/blocks
    unsigned pNum, bNum;
    // number of blocks 
    unsigned pRemain, bRemain;
    PieceInfo *p;

    // TODO
    // let t0 be start of speed measure, v[i] be instant speed at i step
    // then average weighted speed would be Sum[v[t] * ((t_i - t0) + dt), i in [2 .. n]] / Sum[t_i - t0 + dt, i in [2 .. n]] = W / T, where dt = t_i - t_(i - 1)
    // given W ant T at n step, consider (n + 1) step:
    // at (n + 1) step B bytes were received
    // then new speed would be (W + v[n + 1] * (t_(n + 1) - t0 + dt)) / (T + (t' - t0) + dt)
    // which is (W + k * B / (t_(n + 1) - t_n)) / (T + k)
    // where k = 2t_(n + 1) - t0 - t_n
    // so maintain W, T, t0 (start/resume time), t (last measure time)
    // in practice dt sometimes too short, so if dt is less then 2/3 of average deltas
    // then pretend n and (n + 1) steps are just signle step
    //mcs T;
    //unsigned long W, rest;
    //moment t0, t;
    // weighted average speed, bytes per sec
    unsigned speed = 0, cnt;
    void updateSpeed(unsigned bytes);
    bool invalidTime;
    moment t0, t;
    unsigned long B = 0;
};
