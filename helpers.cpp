#include <unistd.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/bencode.hpp>
#include "helpers.h"

#define critical(X, Y) if ((X) == -1) {std::cerr << Y << "\n"; _exit(3);}

namespace lt = libtorrent;

int read_(int fd, void *buf, size_t count) {
    int n;
    size_t curr;
    for (curr = 0; curr < count; curr += n) {
        n = read(fd, (void *)((size_t)buf + curr), count - curr);
        if (n == -1)
            return -1;
        if (n == 0)
            return curr;
    }

    return curr;
}

int write_(int fd, const void *buf, size_t count) {
    int n;
    size_t curr;
    for (curr = 0; curr < count; curr += n) {
        n = write(fd, (void*)((size_t)buf + curr), count - curr);
        if (n == -1)
            return -1;
    }

    return curr;
}

bool handshake(int sock) {
    int n = strlen(shakeMsg);    
    char *reply = new char[n];

    critical(write_(sock, shakeMsg, n), SHAKE_ERR);
    critical(read_(sock, reply, n), SHAKE_ERR);
    bool res = !strcmp(reply, shakeMsg);

    free(reply);
    return res;
}

buf_t* buf_new(size_t capacity) {
    char *new_data = (char*)malloc(capacity);
    if (new_data == NULL) {
        return NULL;
    }

    struct buf_t *result = (buf_t*)malloc(sizeof(buf_t));
    result->capacity = capacity;
    result->size = 0;
    result->data = new_data;
    return result;
}

void buf_free(buf_t* buf) {
    free(buf->data);
    free(buf);
}

ssize_t buf_fill(int fd, buf_t *buf, size_t required) {
    size_t result = 0, tmp;
    while (result < required &&
           (tmp = read(fd, buf->data + buf->size + result, buf->capacity - result - buf->size)) > 0) {
        result += tmp;
    }
    buf->size += result >= 0 ? result : 0;
    return result;
}

ssize_t buf_flush(int fd, buf_t *buf, size_t required) {
    int n;
    if ((n = write(fd, buf->data, required)) == -1) {
	return -1;
    }
    buf->size -= n;
    return n;
}

void printInfo(lt::torrent_info *t) {
    printf("number of pieces: %d\n"
	   "piece length: %d\n"
	   "comment: %s\n"
	   "created by: %s\n"
	   "name: %s\n"
	   "number of files: %d\n"
	   "files:\n"
	   , t->num_pieces()
	   , t->piece_length()
	   , t->comment().c_str()
	   , t->creator().c_str()
	   , t->name().c_str()
	   , t->num_files());
    lt::file_storage const& st = t->files();
    for (int i = 0; i < st.num_files(); ++i) {
	int first = st.map_file(i, 0, 0).piece;
	int last = st.map_file(i, std::max(st.file_size(i)-1, (int64_t)0), 0).piece;
	int flags = st.file_flags(i);
	printf(" %8" PRIx64 " %11" PRId64 " %c%c%c%c [ %5d, %5d ] %7u %s %s %s%s\n"
	       , st.file_offset(i)
	       , st.file_size(i)
	       , ((flags & lt::file_storage::flag_pad_file)?'p':'-')
	       , ((flags & lt::file_storage::flag_executable)?'x':'-')
	       , ((flags & lt::file_storage::flag_hidden)?'h':'-')
	       , ((flags & lt::file_storage::flag_symlink)?'l':'-')
	       , first, last
	       , boost::uint32_t(st.mtime(i))
	       , st.hash(i) != lt::sha1_hash(0) ? lt::to_hex(st.hash(i).to_string()).c_str() : ""
	       , st.file_path(i).c_str()
	       , (flags & lt::file_storage::flag_symlink) ? "-> " : ""
	       , (flags & lt::file_storage::flag_symlink) ? st.symlink(i).c_str() : "");
    }
}
