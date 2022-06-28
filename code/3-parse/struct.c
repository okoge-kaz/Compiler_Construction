#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct student {
    int id;
    char name[20];
    int age;
};

typedef struct point {
    int x;
    int y;
} Point;

void change_(Point p){
    p.x = 100;
    p.y = 200;
}

void change(Point *p){
    p->x = 100;
    p->y = 200;
}

int main(int argc, char* argv[]) {
    struct student s1;
    s1.id = 1;
    strcpy(s1.name, "Taro");
    s1.age = 20;
    printf("%d, %s, %d\n", s1.id, s1.name, s1.age);

    struct student s2 = {0};// これで初期化もできる
    printf("%d, %s, %d\n", s2.id, s2.name, s2.age);

    Point p1 = {1, 2};
    Point p2 = {.x = 3, .y = 4};// これでも初期化もできる
    Point p3 = {.y = 5, .x = 6};// これでも初期化もできる

    // ちゃんとポインタを渡さないと破壊的変更はできない
    change_(p1);
    printf("%d, %d\n", p1.x, p1.y);
    change(&p2);
    printf("%d, %d\n", p2.x, p2.y);

    printf("%d, %d\n", p3.x, p3.y);
    return 0;
}