#include "piece_watcher.h"
#include "txt.cpp"

#define ceilInt(x, y) (x % y ? x / y + 1 : x / y)

void PieceInfo::setSize(unsigned sz) {
    size = ceilInt(sz, BLOCK_SIZE);
    present.resize(size);
    present.shrink_to_fit();
    remain = size;
    finished = size == 0 ? true : false;
}

unsigned PieceInfo::setPresent(unsigned start, unsigned len) {
    unsigned int cnt = 0;
    for (unsigned cur = start; cur < start + len; cur++) {
	cnt += (unsigned)(!present[cur]);
	present[cur] = true;
    }

    remain -= cnt;
    finished = remain == 0;
    return cnt;
}

PieceWatcher::PieceWatcher(const lt::file_storage &fs) {
    invalidTime = true;
    cnt = 0;
    B = 0;
    //rest = 0;
    
    pNum = pRemain = fs.num_pieces();
    len = fs.piece_length();
    int bLen = fs.piece_length() / PieceInfo::BLOCK_SIZE;
    p = new PieceInfo[pNum];
    for (unsigned i = 0; i < pNum - 1; i++) {
	p[i].setSize(bLen);
    }
    p[pNum - 1].setSize(fs.piece_size(pNum - 1));
    bNum = bRemain = (pNum - 1) * bLen +
	ceilInt(fs.piece_size(pNum - 1), PieceInfo::BLOCK_SIZE);
}

PieceWatcher::~PieceWatcher() {
    //    delete p;
}

unsigned getSize(lt::file::iovec_t const *buf, int num_bufs) {
    unsigned result = 0;
    for (int i = 0; i < num_bufs; i++) {
	result += buf[i].iov_len;
    }
    return result;
}

void PieceWatcher::setPresent(lt::file::iovec_t const *buf, int num_bufs, int piece, int offset) {
    if (offset % PieceInfo::BLOCK_SIZE) {
	throw TxtException("Unaligned write\n");
    }
    unsigned sz = getSize(buf, num_bufs);
    unsigned blocks = ceilInt(sz, PieceInfo::BLOCK_SIZE);
    bRemain -= p[piece].setPresent(offset / PieceInfo::BLOCK_SIZE, blocks);
    pRemain -= p[piece].finished;

    updateSpeed(sz);
}

void PieceWatcher::updateSpeed(unsigned bytes) {
    if (invalidTime) {
	invalidTime = false;
	t0 = t = NOW;
	//W = 0;
	//T = mcs(0);
	//speed = 0;
	B = 0;
	return;
    }

    moment now = NOW;
    unsigned long dt = (now - t).count();
    if (dt > 0) {
	speed = bytes * 1000000 / dt;
    }
    B += bytes;
    t = now;
    
    /*moment now = NOW;
    mcs k = now - t0 + now - t;
    unsigned long dt = (now - t).count();
    cnt++;
    if (dt < 2 * (now - t0).count() / cnt / 3) {
	rest += bytes;
	return;
    }
    bytes += rest;
    rest = 0;
    t = now;
	
    //printf("%20lu %20lu\n", dt, W);
    T += k;
    W += k.count() * bytes / dt;
    if (T.count() == 0) {
	speed = 0;
    } else {
	speed = W * 1000000 / T.count();
    }
    printf("%10lu, %10lu, %10u, %10lu\n", speed, dt, bytes, (now - t0).count());*/
}

unsigned PieceWatcher::getEstimated() {
    if (speed == 0 || B == 0) {
	return 0;
    }

    return PieceInfo::BLOCK_SIZE * bRemain * (t - t0).count() / B / 1000000;
}

unsigned PieceWatcher::getProgress() {
    return (bNum - bRemain) * 100 / bNum;
}

unsigned PieceWatcher::getSpeed() {
    return speed;
}

bool PieceWatcher::finished() {
    return bRemain == 0;
}
