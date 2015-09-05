#include <libtorrent/storage.hpp>
#include <libtorrent/storage_defs.hpp>

#define lt libtorrent

// works as default_storage, but forwards mathods calls to given fd
class MirrorStorage : public lt::default_storage {
public:
    MirrorStorage(lt::storage_params const& params, int fd);

    // hidden
    ~MirrorStorage();

    void rename_file(int index, std::string const& new_filename, lt::storage_error& err);
    void release_files(lt::storage_error& err);
    void delete_files(lt::storage_error& err);
    void initialize(lt::storage_error& err);
    int writev(lt::file::iovec_t const* buf, int slot, int offset, int num_bufs, int flags, lt::storage_error& err);
    void write_resume_data(lt::entry& rd, lt::storage_error& err) const;

private:
    int fd;
};
