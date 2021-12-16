#include <stdio.h>
int main(){
    int a1 = 0;
    int b1 = 1;
    int c1 = 2;
    int d1 = 3;
    int a2 = 4;
    int b2 = 5;
    int c2 = 6;
    int d2 = 7;
    for(int i = 0; i < 1000; ++i){
        a2 = a1 + a2;
        b2 = b1 + b2;
        if(i % 100 != 0){ //99%
            c2 = c1 + c2;
            d2 = d1 + d2;
        }
    }
    printf("%d %d %d %d \n", a2, b2, c2, d2);
    return 0;
}