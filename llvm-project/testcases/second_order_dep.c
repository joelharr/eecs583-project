#include <stdio.h>
int main(){
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    int a1 = a + b; 
    int a2 = a1 + b; //<-- G1
    int a3 = a2 + d; //Don't group with either
    int a4 = a1 + d; //<-- G1
    printf("%d %d %d %d\n", a1, a2, a3, a4);
    return 0;
}