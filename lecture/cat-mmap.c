#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

static char *map_file (char *filename)
{
    struct stat sbuf;
    char *ptr;

    int fd = open (filename, O_RDWR);
    if (fd == -1) {
        perror ("open");
        exit (1);
    }

    fstat (fd, &sbuf);

    ptr = mmap (NULL, sbuf.st_size + 1, // +1 for '\0'
                PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED) {
        perror ("mmap");
        exit (1);
    }
    ptr [sbuf.st_size] = '\0';
    return ptr;
}

int main (int argc, char *argv[])
{
    char *ptr, c;

    if (argc != 2) {
        fprintf (stderr, "Usage: %s filename\n", argv[0]);
        exit (1);
    }

    ptr = map_file (argv [1]);

    while ((c = *ptr++) != '\0') {
        putchar (c);
    }
}
