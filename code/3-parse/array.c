#include<stdio.h>

int main(){
    int a[] = {1,2,3,4,5};
    printf("%d, %d, %d, %d\n", a[2], *(a+2), *(2+a), 2[a]);// すべて同じ
    return 0;
}