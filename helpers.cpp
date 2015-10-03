#include <unistd.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <boost/format.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/file.hpp>
#include "helpers.h"
#include "ui/text_area.h"

#define critical(X, Y) if ((X) == -1) {std::cerr << Y << "\n"; _exit(3);}
#define putLn_(X, Y) out->putLn((boost::format(X) % Y).str())

unsigned getSize(lt::file::iovec_t const *buf, int num_bufs) {
    unsigned result = 0;
    for (int i = 0; i < num_bufs; i++) {
	result += buf[i].iov_len;
    }
    return result;
}

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
    int n = strlen(SHAKE_MSG);    
    char *reply = (char*)malloc(sizeof(char) * (n + 1));
    memset(reply, 0, sizeof(char) * (n + 1));

    critical(write_(sock, SHAKE_MSG, n), SHAKE_ERR);
    critical(read_(sock, reply, n), SHAKE_ERR);
    bool res = !strcmp(reply, SHAKE_MSG);

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

void printInfo(boost::shared_ptr<lt::torrent_info> &t, TextArea *out) {
    putLn_("number of pieces: %d", t->num_pieces());
    putLn_("piece length: %d", t->piece_length());
    putLn_("comment: %s", t->comment());
    putLn_("created by: %s", t->creator());
    putLn_("name: %s", t->name());
    putLn_("number of files: %d", t->num_files());
    out->putLn("files:");    
    lt::file_storage const& st = t->files();
    for (int i = 0; i < st.num_files(); ++i) {
	int first = st.map_file(i, 0, 0).piece;
	int last = st.map_file(i, std::max(st.file_size(i)-1, (int64_t)0), 0).piece;
	out->putLn((boost::format("%15uB at [%7u, %7u]: %s")
			   % st.file_size(i)
			   % first % last
		           % st.file_path(i)).str());
    }
}
