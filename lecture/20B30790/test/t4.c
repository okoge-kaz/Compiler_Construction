int   printf ();
void *malloc ();
void  srandom ();
int   random ();
int   atol ();

int mod (int x, int y)
{   return x - (x / y) * y; }

void swap (int *a, int *b)
{   int tmp; tmp = *a; *a = *b; *b = tmp; }

void bubble_sort(int *data, int size)
{
    int i; int j;
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

void make_array (int *data, int size, int seed)
{
    int i; i = 0;
    srandom (seed);
    while (i < size) {
        int j;
        j = random ();
        *(data + i) = mod (j, size);
        i = i + 1;
    }
}

int main (int argc, char **argv)
{
    int i; int *p; int size;
    /*
    int sym_table_dump;
    */
    size = atol (*(argv + 1));
    p = malloc (4 * size);
    make_array (p, size, 0);
    bubble_sort (p, size);
    i = 0;
    while (i < size) {
        printf ("%d\n", *(p + i));
        i = i + 1;
    }
}

