#include <stdio.h>
#include <math.h>
#include <stdint.h>

#define TENS_DIGITS 20

uint64_t limit = 10000000000;
uint64_t answer = 0;
int tens[TENS_DIGITS];
int count_val[TENS_DIGITS];
int count = 0;

int num_decimal_places(uint64_t val) {
  int decimal_places = 0;
  for(int i=0;i<TENS_DIGITS;i++) {
    tens[i] = val;
    if (i>0) {
      tens[i] /= (pow(10,i));
    }
    tens[i] %= (10);
    if (tens[i] != 0) decimal_places = i;
  }
  return decimal_places+1;
}
uint64_t fibonacci(uint64_t curr, uint64_t next) {
  if(curr%2 == 0) answer+=curr;
  if (next > limit) return curr;
  printf("F_%02d: [%d] [%lu]\n",count, num_decimal_places(curr), curr);
  count_val[num_decimal_places(curr)] += 1;
  count++;
  return fibonacci(next, curr+next);
}

int main(int argc, char*argv[]) {
  printf("%lu \n", fibonacci(1,2));;
  printf("Answer: [%3d] [%lu]\n\n", num_decimal_places(answer), answer);
  for(int i=0;i<TENS_DIGITS;i++) {
    if(count_val[i])
    printf("Num with %d decinal places: %d\n", i, count_val[i]);
  }
}
