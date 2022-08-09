int printf ();

long fact (long n)
{
    if (n < 0 || n == 0)
        return 1;
    else
        return n * fact (n - 1);
}

int main ()
{
    printf ("fact (%d) = %ld\n", 10, fact (10));
}
