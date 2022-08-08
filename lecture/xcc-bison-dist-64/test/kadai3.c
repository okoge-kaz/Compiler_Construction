int   printf ();
void *malloc ();
void  srandom ();
int   random ();
long  atol ();

long mod (long x, long y)
{   return x - (x / y) * y; }

void swap (long *a, long *b)
{   long tmp; tmp = *a; *a = *b; *b = tmp; }

void bubble_sort(long *data, long size)
{
    long i; long j;
    i = size - 1;
    while (0 < i) {
        j = 0;
        while (j < i) {
            if (*(data + (j+1)) < *(data + j))
                swap (data + j, data + (j + 1));
            j = j + 1;
        }
        i = i - 1;
    }
}

void make_array (long *data, long size, long seed)
{
    long i; i = 0;
    srandom (seed);

    while (i < size) {
        long j;
        j = random ();
        *(data + i) = mod (j, size);
        i = i + 1;
    }
}

int main (int argc, char **argv)
{
    long i; long *p; long size;
    size = atol (*(argv + 1));
    p = malloc (8 * size);
    make_array (p, size, 0);
    bubble_sort (p, size);
    i = 0;
    while (i < size) {
        printf ("%ld\n", *(p + i));
        i = i + 1;
    }
}

