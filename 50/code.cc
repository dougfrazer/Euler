#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <stack>
using namespace std;

bool prime_sums(int max, int curr_max, int &count, stack<int> &primes) {
  printf("Entered %s(%d,%d,%d,...)\n", __FUNCTION__, max, curr_max, count);
  stack<int> reduced = primes;
  int next = 0;
  if (curr_max == max) return true;
  
  while (!reduced.empty()) {
    next = reduced.top();
    reduced.pop();
    if(next+curr_max <= max) { count++; return prime_sums(max, curr_max+next, count, reduced); }
  }
  return false;

  return 0;
}

int prime_sum_count(int max, stack<int> &primes) {
  stack<int> scope = primes;
  int val = 0;

  while(!scope.empty() && val >= sqrt(max)) {
    val = scope.top();
    scope.pop();
  }
  int count = 0;
  prime_sums(max, 0, count, scope);
  return count;
}

bool is_prime(uint32_t n) {
  for (int i=2; i<=(int)sqrt(n); i++) {
    if(n%i == 0) { 
      return false;
    }
  }
  return true;
}

int main(int argc, char*argv[]) {
  int answer = 0;
  int max = 0,sum=0;
  stack<int> primes;
  for(int i=0;i<100;i++) {
    if (is_prime(i)) primes.push(i);
  }
  for(int i=0;i<100;i++) {
    if(is_prime(i)) max = prime_sum_count(i, primes);
    if(sum > max){ printf("Curr max: %d\n", i); max = sum; answer = i; }
  }
  printf("Answer: %d\n", answer);
}
