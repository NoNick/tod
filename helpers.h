#pragma once

#include <sys/types.h>
#include <boost/shared_ptr.hpp>
#include <libtorrent/torrent_info.hpp>

const char * const SHAKE_ERR = "Hasndshake failed";
const char * const shakeMsg = "Remote hi five!";

int read_(int fd, void *buf, size_t count);
int write_(int fd, const void *buf, size_t count);
bool handshake(int sock);
void printInfo(boost::shared_ptr<libtorrent::torrent_info> t);

struct buf_t {
    size_t capacity, size;
    char *data;
};
buf_t* buf_new(size_t capacity);
void buf_free(buf_t* buf);
ssize_t buf_fill(int fd, buf_t *buf, size_t required);
ssize_t buf_flush(int fd, buf_t *buf, size_t required);
