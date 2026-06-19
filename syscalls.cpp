#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

// QEMU user-mode syscall wrapper for RISC-V
// Provides file I/O support for open(), read(), write(), close()

extern "C" {

int _open(const char *pathname, int flags, int mode) {
    return open(pathname, flags, mode);
}

int _close(int fd) {
    return close(fd);
}

ssize_t _read(int fd, void *buf, size_t count) {
    return read(fd, buf, count);
}

ssize_t _write(int fd, const void *buf, size_t count) {
    return write(fd, buf, count);
}

}
