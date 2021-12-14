int main(){
    int a1 = 0;
    int a2 = 1;
    int a3 = 2;
    int a4 = 4;
    int b1 = 0;
    int b2 = 1;
    int b3 = 2;
    int b4 = 4;

    //Isomorphic instructions, all in a group together
    int c1 = a1 + b1;
    int c2 = a2 + b2;
    int c3 = a3 + b3;
    int c4 = a4 + b4;

    //Isomorphic instructions, but with a dependency on previous instructions
    int d1 = c1 + a1;
    int d2 = c2 + a2;
    int d3 = c3 + a3;
    int d4 = c4 + a4;

    //Non-isomorphic instructions, interleaved ordering
    int e1 = d1 + c1; 
    int e2 = d2 + c2;
    int f1 = d1 * c1;
    int f2 = d2 * c2;
    int e3 = d3 + c3; 
    int e4 = d4 + c4; 
    int f3 = d3 * c3;
    int f4 = d4 * c4;
}