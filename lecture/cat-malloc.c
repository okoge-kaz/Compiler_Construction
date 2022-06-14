#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

off_t getfilesize (int fd)
{
    struct stat sbuf;
    fstat (fd, &sbuf);
    return sbuf.st_size;
}

int main (int argc, char *argv[])
{
    FILE *fp;
    int i;
    off_t file_size;
    char *p;

    if (argc < 2) {
        fprintf (stderr, "usage: %s filename\n", argv [0]);
        exit (1);
    }

    if ((fp = fopen (argv [1], "r")) == NULL) {
        perror ("open");
        exit (1);
    }
    file_size = getfilesize (fileno (fp));
    if ((p = malloc (file_size)) == NULL) {
        perror ("malloc");
        exit (1);
    }
    for (i = 0; i < file_size; i++) {
        p [i] = getc (fp);
    }
    for (i = 0; i < file_size; i++) {
        putchar (p [i]);
    }
    fclose (fp);
}
