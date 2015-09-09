#pragma once

#include <boost/shared_ptr.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/storage_defs.hpp>

// receives torrent file & forward downloaded data
class Obtainer {
public:
    Obtainer(int socket);
    ~Obtainer();
    void run();
private:
    int sock;
    const char *shakeMsg = "Remote hi five!";
    // reads size sz and then downloads buffer of size sz
    boost::shared_ptr<libtorrent::torrent_info> getTorrent();

    const char *TOR_GET_ERR = "Error occurred during receiving torrent file";
    const char *SHAKE_ERR = "Hasndshake failed";
};

