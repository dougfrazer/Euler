#include <stdio.h>

int limit = 4000000;
int answer = 0;

int fibonacci(int curr, int next) {
  if(curr%2 == 0) answer+=curr;
  if (next > limit) return curr;
  printf("%d ", curr);
  return fibonacci(next, curr+next);
}

int main(int argc, char*argv[]) {
  printf("%d \n", fibonacci(1,2));;
  
  printf("Answer: %d\n", answer);
}

