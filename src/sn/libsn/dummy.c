#include <sys/stat.h>

extern int InitializeUART(int);
extern int WriteUARTN(const void *, unsigned int);
extern int lbl_80152718(int);
extern int lbl_80152720(int, char *, int);
extern int lbl_80152728(int, char *, int);
extern int lbl_80152730(int, int, int);

extern int lbl_801FD048;

int fn_8018AB8C(char *buf, int len) {
    if (lbl_801FD048) {
        lbl_801FD048 = 0;
        InitializeUART(0);
    }
    WriteUARTN(buf, len);
    return len;
}

int fn_8018ABE8(int fd, char *buf, int len) {
    if (fd == 1 || fd == 2) {
        return fn_8018AB8C(buf, len);
    }
    if (fd == 0) {
        return -1;
    }
    return lbl_80152728(fd, buf, len);
}

int _write(int fd, char *buf, int len) {
    return fn_8018ABE8(fd, buf, len);
}

int _close(int fd) {
    return lbl_80152718(fd);
}

int _fstat(int fd, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _lseek(int fd, int ptr, int dir) {
    return lbl_80152730(fd, ptr, dir);
}

int _read(int fd, char *buf, int len) {
    return lbl_80152720(fd, buf, len);
}
