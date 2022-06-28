#include <stdio.h>
#include <stdlib.h>

enum ip_address {
    ipv4_address,
    ipv4_address_bytes,
};

enum one_two_three {
    one = 1,
    two = 2,
    three = 3,
};

// 列挙定数の重複は許されない
typedef enum {
    ONE = 1,
    TWO = 2,
    THREE = 3,
} ONE_TWO_THREE;

int main(int argc, char **argv) {
    enum one_two_three tmp = one;
    printf("%d\n", tmp);

    int tmp2 = two;
    printf("%d\n", tmp2);

    ONE_TWO_THREE tmp3 = THREE;
    printf("%d\n", tmp3);

    return 0;
}