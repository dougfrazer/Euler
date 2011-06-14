#include <stdio.h>

int one(int a) {
  if (a%3 == 0 || a%5 == 0) return a;
  return 0;
}

int main(int argc, char*argv[]) {
  int sum = 0;
  for (int i=0;i<1000;i++) {
    sum += one(i);
  }
  printf("Answer: %d\n", sum);
}
