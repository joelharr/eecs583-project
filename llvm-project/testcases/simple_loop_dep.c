int main(){
    int a = 0;
    int b = 1;
    int c = 2;
    int d = 3;
    for(int i = 0; i < 10; i++){
        a = b + c; //These two instructions should be vectorizable at width 2, but not at width 4 (unrolled)
        b = d + c;
    }
    return a;
}