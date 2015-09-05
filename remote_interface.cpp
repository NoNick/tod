#include <iostream>
#include <exception>
#include <libtorrent/storage.hpp>
#include "helpers.h"

#define success(X, Y) if ((X) == -1) { \
                          std::cerr << Y << "\n"; \
                          return -1; }
#define successE(X) if ((X) == -1) throw std::exception();

int callRemote(int fd, Func f) {
    success(write_(fd, &f, sizeof(Func)), REMOTE_ERR);
    return 0;
}

template <typename ... Args>
int callRemote(int fd, Func f, std::list<size_t> size, Args ... args) {
    success(write_(fd, &f, sizeof(Func)), REMOTE_ERR);
    return sendArgs(fd, size, args...);
}
	
template <typename T>
int sendArgs(int fd, std::list<size_t> size, T arg) {
    success(write_(fd, &size.front(), sizeof(size_t)), REMOTE_ERR);
    success(write_(fd, &arg, size.front()), REMOTE_ERR);
    return 0;
}

template <typename T, typename ... Args>
int sendArgs(int fd, std::list<size_t> size, T arg, Args ... args) {
    success(write_(fd, &size.front(), sizeof(size_t)), REMOTE_ERR);
    success(write_(fd, &arg, size.front()), REMOTE_ERR);
    size.pop_front();
    return sendArgs(fd, size, args...);
}

template <typename T>
T* receiveArg(int fd) {
    size_t sz;
    successE(read_(fd, &sz, sizeof(size_t)));
    T *arg = new T();
    successE(read_(fd, arg, sz));
    return arg;
}

lt::file::iovec_t* receiveVec(int fd) {
    size_t sz;
    successE(read_(fd, &sz, sizeof(size_t)));
    lt::file::iovec_t *v = new lt::file::iovec_t;
    v->iov_base = malloc(sz);
    successE(read_(fd, v->iov_base, sz));
    v->iov_len = sz;
    return v;
}

// callN takes function name and its argument types, then fetches arguments and calls the function
#define get(T, N) T* N = receiveArg<T>(fd);
#define call0(F) lt::storage_error err; F(err);
#define call1(F, A) lt::storage_error err; get(A, a); F(*a, err); delete a;
#define call2(F, A, B) lt::storage_error err; get(A, a); get(B, b); F(*a, *b, err); delete a; delete b;

// returns 0 for success
//         1 for network error
//         2 for storage error
int listen(int fd, lt::default_storage &def) {
    Func f;
    read_(fd, &f, sizeof(Func));

    switch(f) {
    case rename_file: {
	call2(def.rename_file, int, std::string);
	break; }
    case release_files: {
	call0(def.release_files);
	break; }
    case delete_files: {
	call0(def.delete_files);
	break; }
    case initialize: {
	call0(def.initialize);
	break; }
    case writev_file: {
	lt::storage_error err;
	lt::file::iovec_t *v = receiveVec(fd);
	get(int, slot);
	get(int, offset);
	get(int, num_bufs);
	get(int, flags);
	def.writev(v, *slot, *offset, *num_bufs, *flags, err);
	free(v->iov_base);
	delete v;
	delete slot;
	delete offset;
	delete num_bufs;
	delete flags;
	break; }
    case write_resume: {
	call1(def.write_resume_data, lt::entry);
	break; }
    }
	  
    return 0;
}
