#include <unistd.h>
#include <string.h>
#include <libtorrent/torrent_info.hpp>
#include "obtainer.h"
#include "helpers.h"

#define critical(X, Y) if ((X) == -1) {std::cerr << Y << "\n"; _exit(3);}

namespace lt = libtorrent;

Obtainer::Obtainer(int socket) : sock(socket) {}

Obtainer::~Obtainer() {
    close(sock);
}

lt::torrent_info* Obtainer::getTorrent() {
    char *b = new char[4];;
    critical(read_(sock, b, 4) == 4, TOR_GET_ERR);
    size_t sz = *((int32_t*)b);
    free(b);

    if (sz > 100 * 1024 * 1024) {
	std::cout << "Warning: torrent file larger than 100MB\n";
    }
	
    char *torrent = new char[sz];
    critical((size_t)read_(sock, torrent, sz) == sz, TOR_GET_ERR);

    lt::torrent_info *info = new lt::torrent_info(torrent, sz);
    free(torrent);
    return info;
}

void Obtainer::run() {
    critical(handshake(sock), SHAKE_ERR);
    lt::torrent_info *t = getTorrent();
    char ih[41];
    lt::to_hex((char const*)&t->info_hash()[0], 20, ih);
    printf("number of pieces: %d\n"
	   "piece length: %d\n"
	   "info hash: %s\n"
	   "comment: %s\n"
	   "created by: %s\n"
	   "name: %s\n"
	   "number of files: %d\n"
	   "files:\n"
	   , t->num_pieces()
	   , t->piece_length()
	   , ih
	   , t->comment().c_str()
	   , t->creator().c_str()
	   , t->name().c_str()
	   , t->num_files());
    delete t;
}
