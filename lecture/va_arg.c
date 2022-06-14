#include <stdio.h>
#include <stdarg.h>
int sum (int num, ...)
{
    va_list ap;
    int i, n, total = 0;
    va_start (ap, num);
    for (i = 0;i < num; i++) {
        n = va_arg (ap, int);
        printf ("%d, %d\n", i, n);
        total += n;
    }
    va_end (ap);
    return total;
}
int main (void)
{
    printf ("total=%d\n", sum (0));
    printf ("total=%d\n", sum (3, 10, 20, 30));
    printf ("total=%d\n", sum (5, 10, 20, 30, 40, 50));
}
