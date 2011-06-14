#include <stdio.h>
#include <math.h>
#include <stdint.h>

bool is_prime(uint32_t n) {
  for (int i=2; i<=(int)sqrt(n); i++) {
    if(n%i == 0) { 
      return false;
    }
  }
  return true;
}

int main(int argc, char*argv[]) {
  int count = 1;
  int i = 2;
  while (true) {
    if(is_prime(i)){
      count++;
      if(count==10001) break;
    }
    i++;
  }
  printf("Answer: %d\n", i);
}
