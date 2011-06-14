#include <stdio.h>

bool divis_by_all(int n, int limit) {
  for (int i =limit/2;i<limit;i++){
    if(n%i) return false;
  }
  return true;
}

int main(int argc, char*argv[]) {
  int answer = 20;
  while(!divis_by_all(answer,20)) {
    answer+=20;
  }
  printf("Answer: %d\n", answer);
}
