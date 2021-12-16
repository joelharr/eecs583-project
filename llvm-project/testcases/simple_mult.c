int main(){
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;

    int a1 = a * b; //<-- G1
    int a2 = a1 * b; //<-- G2
    int e1 = c * d; //<-- G1
    int e2 = e1 * d; //<-- G2
    int e3 = e2 / e1;
    printf("%d %d %d %d %d\n", a1, a2, e1, e2, e3);
    return 0;
}
