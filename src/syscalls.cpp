#include <errno.h>
#include <sys/stat.h>

extern "C" {
extern int __io_putchar(int ch) __attribute__((weak));
extern int __io_getchar(void) __attribute__((weak));

__attribute__((weak)) int _read(int file, char* ptr, int len) {
    int DataIdx;
    (void)file;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        *ptr++ = static_cast<char>(__io_getchar());
    }

    return len;
}

__attribute__((weak)) int _write(int file, char* ptr, int len) {
    int DataIdx;
    (void)file;

    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}

int _getpid(void) {
    return 1;
}

int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

int _close(int file) {
    (void)file;
    return -1;
}

int _fstat(int file, struct stat* st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    (void)file;
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    (void)file;
    (void)ptr;
    (void)dir;
    return 0;
}

int _open(char* path, int flags, ...) {
    (void)path;
    (void)flags;
    return -1;
}

int _stat(char* file, struct stat* st) {
    (void)file;
    st->st_mode = S_IFCHR;
    return 0;
}
}
