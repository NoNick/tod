#include <unistd.h>
#include <string.h>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/storage_defs.hpp>
#include <libtorrent/entry.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/session.hpp>
#include <boost/thread/thread.hpp>
#include "mirror_storage.h"
#include "obtainer.h"
#include "helpers.h"

#define critical(X, Y) if ((X) == -1) {std::cerr << Y << "\n"; _exit(3);}

#define lt libtorrent

#define pb push_back

Obtainer::Obtainer(int socket) : sock(socket) {}

Obtainer::~Obtainer() {
    close(sock);
}

boost::shared_ptr<lt::torrent_info> Obtainer::getTorrent() {
    char *b = new char[4];;
    critical(read_(sock, b, 4) == 4, TOR_GET_ERR);
    size_t sz = *((int32_t*)b);
    free(b);

    if (sz > 100 * 1024 * 1024) {
	std::cout << "Warning: torrent file larger than 100MB\n";
    }
	
    char *torrent = new char[sz];
    critical((size_t)read_(sock, torrent, sz) == sz, TOR_GET_ERR);

    boost::shared_ptr<lt::torrent_info> info(new lt::torrent_info(torrent, sz));
    free(torrent);
    return info;
}

int fd;
lt::storage_interface* mirrorConstructor(lt::storage_params const& params) {
    return new MirrorStorage(params, fd);
}

void Obtainer::run() {
    critical(handshake(sock), SHAKE_ERR);
    boost::shared_ptr<lt::torrent_info> t = getTorrent();
    printInfo(t);

    lt::settings_pack sett;
    sett.set_str(lt::settings_pack::listen_interfaces, "0.0.0.0:6881");
    lt::session s(sett);
    lt::error_code ec;
    if (ec) {
	std::cerr << "failed to open listen socket: " << ec.message() << "\n";
	return;
    }
    fd = sock;
    lt::add_torrent_params p((lt::storage_constructor_type)mirrorConstructor);
    p.save_path = "./";
    p.ti = t;
    if (ec) {
	std::cerr << ec.message() << "\n";
	return;
    }
    s.add_torrent(p, ec);
    if (ec) {
	std::cerr << ec.message() << "\n";
	return;
    }

    while (true)
    pause();
}
