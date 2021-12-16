#include <stdio.h>

int main()
{

  int in[1000];
  int i,j;

  for (i = 0; i < 1000; i++)
  {
    in[i] = i+2;
    in[i+1] = i+1;
  }
  for(i = 0; i < 1000; i++)
    printf("%d ", i);
  
  return 1;
}

