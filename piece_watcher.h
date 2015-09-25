#include <libtorrent/file.hpp>
#include <libtorrent/file_storage.hpp>

namespace lt = libtorrent;

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

    // assume piece has 8 KB blocks (in case of 16 KB blocks let's mark two blocks instead of one)
    static const unsigned BLOCK_SIZE = 8 * 1024;
};

class PieceWatcher {
public:
    PieceWatcher(const lt::file_storage &fs);
    virtual ~PieceWatcher();
    void setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset);
    // percentage of present blocks
    unsigned getProgress();
    bool finished();
private:
    // piece length (except for the last one)
    unsigned len;
    // number of pieces/blocks
    unsigned pNum, bNum;
    // number of blocks 
    unsigned pRemain, bRemain;
    PieceInfo *p;
};
