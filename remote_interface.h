// handmade rmi for mirrorStorage
// be careful with big/little-endian encodings!
#include <stdarg.h>
#include <functional>
#include <libtorrent/storage.hpp>
#include "ui/progress.h"

#define lt libtorrent

const char * const REMOTE_ERR = "Connection problem, aborting query.";

enum Func {
    rename_file, release_files, delete_files, initialize,
    writev_file, write_resume};

class Remote {
public:
    Remote(int fd);
    ~Remote();
    int callRemote(Func f);
    void sendVec(lt::file::iovec_t const *bufs, int num_bufs);
    template <typename ... Args>
	int callRemote(Func f, std::list<size_t> size, Args ... args);
    int listenStorage(lt::default_storage &ds, ProgressWatcher *pw);
private:
    template <typename T>
	int sendArgs(std::list<size_t> size, T arg);
    template <typename T, typename ... Args>
	int sendArgs(std::list<size_t> size, T arg, Args ... args);
    int fd;
    // TODO: queue here
};

// template issue, moving template functions in single translation unit
//#include "remote_interface.cpp"
