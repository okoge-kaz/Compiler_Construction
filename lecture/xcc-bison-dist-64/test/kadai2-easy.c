int printf();
long i;

int main() {
    i = 0;
    while (i < 11) {
        if (i == 2) {
            printf("i == 2\n");
            if (i / 2 == 1) {
                printf("2 / 2 == 1\n");
            }
        } else if (i * i == i) {
            printf("%d * %d == %d\n", i, i, i);
        } else if (2 < i && i < 8) {
            printf("2 < %d < 8\n", i);
        } else {
            printf("else\n");
        }
        i = i + 1;
    }

    while (i == 2 || i == 11) {
        printf("test: %d\n", i);
        i = i + 1;
    }
    printf("i: %d\n", i);
}
