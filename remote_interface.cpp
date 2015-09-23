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
    success(write_(fd, &f, sizeof(Func)), REMOTE_ERR);
    return 0;
}

template <typename ... Args>
int Remote::callRemote(Func f, std::list<size_t> size, Args ... args) {
    std::cout << "making call " << f << "..\n";
    success(write_(fd, &f, sizeof(Func)), REMOTE_ERR);
    return sendArgs(size, args...);
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

void Remote::sendVec(lt::file::iovec_t const *bufs, int num_bufs) {
    for (int i = 0; i < num_bufs; i++) {
	successE(write_(fd, &(bufs[i].iov_len), sizeof(size_t)));
	successE(write_(fd, bufs[i].iov_base, bufs[i].iov_len));
    }		
}

lt::file::iovec_t* receiveVec(int fd, int num_bufs) {
    lt::file::iovec_t *bufs = new lt::file::iovec_t[num_bufs];
    for (int i = 0; i < num_bufs; i++) {
	size_t sz;
	successE(read_(fd, &sz, sizeof(size_t)));
	bufs[i].iov_base = malloc(sz);
	successE(read_(fd, bufs[i].iov_base, sz));
	bufs[i].iov_len = sz;
    }
    return bufs;
}

void delVec(lt::file::iovec_t* bufs, int num_bufs) {
    for (int i = 0; i < num_bufs; i++) {
	free(bufs[i].iov_base);
    }
    delete[] bufs;
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
	get(int, num_bufs);
	get(int, piece);
	get(int, offset);
	get(int, flags);
	lt::file::iovec_t *bufs;
	try {
	    bufs = receiveVec(fd, *num_bufs);
	} catch (std::exception e) {
	    std::cout << "bufs exception\n";
	    _exit(1);
	}
	//	do {
	std::cout << "piece #" << *piece << ": " << def.writev(bufs, *num_bufs, *piece, *offset, *flags, err) << " bytes are written\n";
	    //} while (err.operation != lt::storage_error::file_operation_t::none);
	delVec(bufs, *num_bufs);
	delete piece;
	delete offset;
	delete num_bufs;
	delete flags;
	result;
	break; }
    case write_resume: {
	call1(def.write_resume_data, lt::entry);
	break; }
    }
	  
    return 0;
}

template int Remote::callRemote(Func, std::list<size_t>, int*, const char*);
template int Remote::callRemote(Func, std::list<size_t>, int*, int*, int*, int*);
