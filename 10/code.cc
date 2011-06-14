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
  uint64_t answer = 2;
  for(int i=3;i<2000000;i+=2) {
    if(is_prime(i)){
      answer += i;
    }
  }
  printf("Answer: %lu\n", answer);
}
