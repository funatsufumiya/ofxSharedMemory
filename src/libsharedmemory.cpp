#include "libsharedmemory.hpp"


// Windows shared memory implementation
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

namespace lsm {

#include <io.h>  // CreateFileMappingA, OpenFileMappingA, etc.

Memory::Memory(const std::string path, const std::size_t size, const bool persist) : _path(path), _size(size), _persist(persist) {};

Error Memory::createOrOpen(const bool create) {
    if (create) {
        DWORD size_high_order = 0;
        DWORD size_low_order = static_cast<DWORD>(_size);

        _handle = CreateFileMappingA(INVALID_HANDLE_VALUE,  // use paging file
                                        NULL,                  // default security
                                        PAGE_READWRITE,        // read/write access
                                        size_high_order, size_low_order,
                                        _path.c_str()  // name of mapping object
        );

        if (!_handle) {
            return kErrorCreationFailed;
        }
    } else {
      _handle = OpenFileMappingA(FILE_MAP_READ, // read access
                                 FALSE,         // do not inherit the name
                                 _path.c_str()  // name of mapping object
      );

      // TODO: Windows has no default support for shared memory persistence
      // see: destroy() to implement that

        if (!_handle) {
            return kErrorOpeningFailed;
        }
    }

    // TODO: might want to use GetWriteWatch to get called whenever
    // the memory section changes
    // https://docs.microsoft.com/de-de/windows/win32/api/memoryapi/nf-memoryapi-getwritewatch?redirectedfrom=MSDN

    const DWORD access = create ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ;
    _data = MapViewOfFile(_handle, access, 0, 0, _size);

    if (!_data) {
        return kErrorMappingFailed;
    }
    return kOK;
}

void Memory::destroy() {

  // TODO: Windows needs priviledges to define a shared memory (file mapping)
  // OBJ_PERMANENT; furthermore, ZwCreateSection would need to be used.
  // Instead of doing this; saving a file here (by name, temp dir)
  // and reading memory from file in createOrOpen seems more suitable.
  // Especially, because files can be removed on reboot using:
  // MoveFileEx() with the MOVEFILE_DELAY_UNTIL_REBOOT flag and lpNewFileName
  // set to NULL.
}

void Memory::close() {
    if (_data) {
        UnmapViewOfFile(_data);
        _data = nullptr;
    }
    CloseHandle(_handle);
}

Memory::~Memory() {
    close();
    if (!_persist) {
      destroy();
    }
}

} // namespace lsm

// NOTE: Other implementations are written on .hpp file

#endif // defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)