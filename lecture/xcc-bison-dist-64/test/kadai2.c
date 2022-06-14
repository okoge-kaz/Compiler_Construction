int printf ();

long mrn (long n)
{
    if (n < 0 || n == 0)
        return 0;
    else
        return 10 * mrn (n - 1) + n;
}

int main ()
{
  long i;

  i = 0;
  while (i < 11) {
    printf ("mrn(%ld) = %ld\n", i, mrn(i));
    i = i+1;
  }
}
