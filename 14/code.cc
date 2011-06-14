#include <stdio.h>
#include <stdint.h>

static inline uint64_t compute_even(uint64_t n){ return n/2; }
static inline uint64_t compute_odd(uint64_t n){ return 3*n + 1; }

int collatz_chain(int n) {
  uint64_t val = n;
  int count = 1;
  while(true) {
    if (val == 1) break;
    if(val%2) val = compute_odd(val);
    else val = compute_even(val);
    count++;
  }
  return count;
}

int main(int argc, char*argv[]) {
  int answer = 0;
  int max = 0;
  int temp = 0;
  
  for(int i=10;i<1000000;i++) {
    temp = collatz_chain(i);
    if(temp > max){ answer = i; max=temp;}
  }
  printf("Answer: %d, max rep: %d\n", answer, max); return 0;
}
