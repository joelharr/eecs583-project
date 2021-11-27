#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//Attempts at writing code snippets that get vectorized by slp-vectorizer
int main() {
    int A[4] = {1, 2, 3, 4};
    srand(time(NULL));
    int r1 = rand();
    int r2 = rand();
    int r3 = rand();
    int r4 = rand();

    A[0]*=r1; //SLP vectorization
    A[1]*=r2;
    A[2]*=r3;
    A[3]*=r4;

    for(int i = 0; i < 4; i++){ //Loop vectorization
        A[i]++;
    }

    for(int i = 0; i < 4; i++){
        printf("i: %d", A[i]);
    }
    return 0;
}

void foo(int a1, int a2, int b1, int b2, int *A) {
    A[0] += a1*(a1 + b1);
    A[1] += a2*(a2 + b2);
    A[2] += a1*(a1 + b1);
    A[3] += a2*(a2 + b2);
}

void foo2(int* a, int* b, int* c){
    for(int i = 0; i < 1000; i++){
        a[i]   = b[i]  *c[i]   + a[i];
        a[i+1] = b[i+1]*c[i+1] + a[i+1];
        a[i+2] = b[i+2]*c[i+2] + a[i+2];
        a[i+3] = b[i+3]*c[i+3] + a[i+3];
        a[i+4] = b[i+4]*c[i+4] + a[i+4];
        a[i+5] = b[i+5]*c[i+5] + a[i+5];
        a[i+6] = b[i+6]*c[i+6] + a[i+6];
        a[i+7] = b[i+7]*c[i+7] + a[i+7];
    }
}