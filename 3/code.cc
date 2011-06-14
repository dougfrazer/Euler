#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <list>
using namespace std;

void add_factor(int n, list<int> &factors) {
  for(list<int>::iterator it = factors.begin(); it != factors.end(); it++) {
    if(*it == n) return;
  }
  factors.push_back(n);
}

bool prime_factors(uint64_t n, list<int> &factors) {
  bool prime = true;
  if (n == 1) return true;
  for (int i=2;i<=(int)sqrt(n);i++) {
    if(n%i == 0) { 
      prime = false;
      prime_factors(n/i, factors);
      prime_factors(i, factors);
    }
  }
  if(prime) add_factor(n, factors);
  return prime;
}

void print_factors(list<int> &factors) {
  printf("Factors: ");
  for(list<int>::iterator it = factors.begin(); it != factors.end(); it++) {
    printf(" %d", *it);
  }
  printf("\n");
}


int main(int argc, char*argv[]) {
  int answer = 0;
  list<int> factors;
  prime_factors(600851475143, factors); 
  print_factors(factors);
}
