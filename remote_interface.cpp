#include <iostream>
#include <exception>
#include <libtorrent/storage.hpp>
#include "helpers.h"
#include "synchronize.h"
#include "remote_interface.h"

#define success(X, Y) if ((X) == -1) { \
                          std::cerr << Y << "\n"; \
                          return -1; }
#define successE(X) if ((X) == -1) throw std::exception();

Remote::Remote(int fd) : fd(fd) {}

Remote::~Remote() { close(fd); }

int Remote::callRemote(Func f) {
    synchronized(this) {
	success(write_(fd, &f, sizeof(Func)), REMOTE_ERR);
    }
    return 0;
}

template <typename ... Args>
int Remote::callRemote(Func f, std::list<size_t> size, Args ... args) {
    synchronized(this) {
	std::cout << "making call " << f << "..\n";
	success(write_(fd, &f, sizeof(Func)), REMOTE_ERR);
	return sendArgs(size, args...);
    }
    return 0;
}
	
template <typename T>
int Remote::sendArgs(std::list<size_t> size, T arg) {
    std::cout << *(int*)arg << "\n";
    success(write_(fd, &size.front(), sizeof(size_t)), REMOTE_ERR);
    success(write_(fd, arg, size.front()), REMOTE_ERR);
    return 0;
}

template <typename T, typename ... Args>
int Remote::sendArgs(std::list<size_t> size, T arg, Args ... args) {
    std::cout << *(int*)arg << " ";
    success(write_(fd, &size.front(), sizeof(size_t)), REMOTE_ERR);
    success(write_(fd, arg, size.front()), REMOTE_ERR);
    size.pop_front();
    return sendArgs(size, args...);
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
#define result std::cout << err.operation_str() << "\n";
#define call0(F) lt::storage_error err; F(err);
#define call1(F, A) lt::storage_error err; get(A, a); F(*a, err); delete a;
#define call2(F, A, B) lt::storage_error err; get(A, a); get(B, b); F(*a, *b, err); delete a; delete b;

// returns 0 for success
//         1 for network error
//         2 for storage error
int Remote::listenStorage(lt::default_storage &def) {
    Func f;
    read_(fd, &f, sizeof(Func));
    std::cout << f << "\n";

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
	std::cout << "initializing...\n";
	call0(def.initialize);
	result;
	break; }
    case writev_file: {
	lt::storage_error err;
	lt::file::iovec_t *v = receiveVec(fd);
	get(int, slot);
	get(int, offset);
	get(int, num_bufs);
	get(int, flags);
	std::cout << v->iov_base << "\n" << *slot << ", " << *offset << ", " << *num_bufs << ", " << *flags << "\n";
	def.writev(v, *slot, *offset, *num_bufs, *flags, err);
	free(v->iov_base);
	delete v;
	delete slot;
	delete offset;
	delete num_bufs;
	delete flags;
	std::cout << "received slot #" << *slot << "\n";
	result;
	break; }
    case write_resume: {
	call1(def.write_resume_data, lt::entry);
	break; }
    }
	  
    return 0;
}

template int Remote::callRemote(Func, std::list<size_t>, int*, const char*);
template int Remote::callRemote(Func, std::list<size_t>, void*, int*, int*, int*, int*);
