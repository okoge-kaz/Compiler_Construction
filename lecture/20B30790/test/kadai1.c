int printf ();
long i;
long sum;

int main()
{
  i = 5;
  sum = 0;
  while (0 < i) {
    printf ("i = %ld\n", i);
    sum = sum + i;
    i = i - 1;
  }
  printf ("sum = %ld\n", sum);
}
