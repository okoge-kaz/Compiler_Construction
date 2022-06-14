int printf ();
int i;
int sum;

int main()
{
  i = 5;
  sum = 0;
  while (0 < i) {
    printf ("i = %d\n", i);
    sum = sum + i;
    i = i - 1;
  }
  printf ("sum = %d\n", sum);
}
