#include <unistd.h>
#include <string.h>
#include <iostream>
#include "helpers.h"

#define critical(X, Y) if ((X) == -1) {std::cerr << Y << "\n"; _exit(3);}

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

