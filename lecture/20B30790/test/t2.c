int printf ();

int main ()
{
    int i; int sum;

    sum = 0;
    i = 0;
    while (i < 100) {
    	printf ("i = %d\n", i);
        sum = sum + i;
        i   = i + 1;
    }

    printf ("sum = %d\n", sum);
}
