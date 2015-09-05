// handmade rmi for mirrorStorage
// be careful with big/little-endian encodings!
#include <stdarg.h>
#include <functional>
#include <libtorrent/storage.hpp>

#define lt libtorrent

const char * const REMOTE_ERR = "Connection problem, aborting query.";

enum Func {
    rename_file, release_files, delete_files, initialize,
    writev_file, write_resume};

int callRemote(int fd, Func f);
template <typename ... Args>
int callRemote(int sock, Func f, std::list<size_t> size, Args ... args);
template <typename T>
    int sendArgs(int fd, std::list<size_t> size, T arg);
template <typename T, typename ... Args>
    int sendArgs(int fd, std::list<size_t> size, T arg, Args ... args);

int listen(int sock, lt::default_storage &ds);

// template issue, moving template functions in single translation unit
#include "remote_interface.cpp"
