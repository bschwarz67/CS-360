#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long unsigned int LUI;

typedef struct {
  char b;
  int i;
} Char_Int;

int main()
{
  Char_Int x;
  x.b = 'c';
  x.i = 7;
  printf("%d\n", x.i);
  printf("0x%lx\n", (unsigned long) &x);
  printf("%lu\n", (unsigned long) &x);

  long i = 140733481930528;
  long unsigned int j;
  long int k;
  printf("0x%016lx\n",i);
  printf("%d\n", sizeof(unsigned int));
  printf("%d\n", sizeof(unsigned long int));
  return 0;
 
}
