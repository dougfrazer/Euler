#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <list>
using namespace std;

class Prime {
  
  public:
    bool is_prime(uint64_t);
    bool find_factors(uint64_t, list<int> &);
    void print_factors(list<int> &);

  private:
    void add_factor(int , list<int> &);
    bool prime_factors(uint64_t, list<int> &, bool);
};


bool Prime::is_prime(uint64_t n) {
  list<int> null;
  return prime_factors(n, null, false);
}

bool Prime::find_factors(uint64_t n, list<int> &factors) {
  return prime_factors(n, factors, true);
}

bool Prime::prime_factors(uint64_t n, list<int> &factors, bool return_list) {
  bool prime = true;
  if (n == 1) return true;
  for (int i=2;i<=(int)sqrt(n);i++) {
    if(n%i == 0) { 
      prime = false;
      prime_factors(n/i, factors, return_list);
      prime_factors(i, factors, return_list);
    }
  }
  if(prime && return_list) add_factor(n, factors);
  return prime;
}

void Prime::print_factors(list<int> &factors) {
  printf("Factors: ");
  for(list<int>::iterator it = factors.begin(); it != factors.end(); it++) {
    printf(" %d", *it);
  }
  printf("\n");
}

void Prime::add_factor(int n, list<int> &factors) {
  for(list<int>::iterator it = factors.begin(); it != factors.end(); it++) {
    if(*it == n) return;
  }
  factors.push_back(n);
}
