#include <stdio.h>
int main(){
    int a1 = 0;
    int b1 = 1;
    int a2 = 4;
    int b2 = 5;
    for(int i = 0; i < 1000; ++i){
        a2 = a1 - a2;
        if(i % 10 != 0){ //90%
            b2 = b1 - b2;
        }
    }
    printf("%d %d \n", a2, b2);
    return 0;
}