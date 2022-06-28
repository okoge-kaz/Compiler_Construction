#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

off_t get_file_size(int fd) {
    // fileのサイズは結構大きくなるので int ではなく off_t で取得する
    struct stat sbuf;
    fstat(fd, &sbuf);
    return sbuf.st_size;
}

int main(int argc, char *argv[]) {
    FILE *fp; // file pointer
    off_t file_size; // file size
    char *buf; // buffer

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen");
        exit(1);
    }

    file_size = get_file_size(fileno(fp));
    buf = (char *)malloc(file_size);
    if (buf == NULL) {
        perror("malloc");
        exit(1);
    }

    for(int i=0; i<file_size; i++) {
        buf[i] = fgetc(fp);
    }
    for(int i=0; i<file_size; i++) {
        printf("%c", buf[i]);
    }

    free(buf);
    fclose(fp);
    return 0;
}