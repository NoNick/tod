#include "mirror_storage.h"

#define ds lt::default_storage
#define pb push_back

MirrorStorage::MirrorStorage(lt::storage_params const& params, int fd) :
    default_storage(params), r(fd){}

MirrorStorage::~MirrorStorage() {}

void MirrorStorage::rename_file(int index, std::string const& new_filename, lt::storage_error& err) {
    std::list<size_t> sizes;
    sizes.pb(sizeof(int));
    sizes.pb(new_filename.size());
    r.callRemote<int*, const char*>(Func::rename_file, (std::list<size_t>)sizes, &index, new_filename.c_str());
    return ds::rename_file(index, new_filename, err);
}

void MirrorStorage::release_files(lt::storage_error& err) {
    r.callRemote(Func::release_files);
    return ds::release_files(err);
}

void MirrorStorage::delete_files(lt::storage_error& err) {
    r.callRemote(Func::delete_files);
    return ds::delete_files(err);
}

void MirrorStorage::initialize(lt::storage_error& err) {
    r.callRemote(Func::initialize);
    return ds::initialize(err);
}

int MirrorStorage::writev(lt::file::iovec_t const* buf, int slot, int offset, int num_bufs, int flags, lt::storage_error& err) {
    std::list<size_t> sizes;
    sizes.pb(buf->iov_len);
    sizes.pb(sizeof(int));
    sizes.pb(sizeof(int));
    sizes.pb(sizeof(int));
    sizes.pb(sizeof(int));
    r.callRemote<void*, int*, int*, int*, int*>(Func::writev_file, sizes, buf->iov_base, &slot, &offset, &num_bufs, &flags);
    return ds::writev(buf, slot, offset, num_bufs, flags, err);
}

void MirrorStorage::write_resume_data(lt::entry& rd, lt::storage_error& err) const {
    return ds::write_resume_data(rd, err);
}
