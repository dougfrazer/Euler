#include <stdio.h>
#include <math.h>

int is_palindromic(int val) {
  int tens[10];
  int decimal_places = 10;

  for(int i=0;i<10;i++) {
    tens[i] = val;
    if (i>0) {
      tens[i] /= (pow(10,i));
    }
    tens[i] %= (10);
    if (tens[i] != 0) decimal_places = i;
  }
  
  for(int i=0;i<(decimal_places+1)/2;i++) {
    if (tens[i] == tens[decimal_places-i])
      continue;
    return false;
  }
  return true;
}

int main(int argc, char*argv[]) {
  int answer = 0;
  
  for(int i=100;i<1000;i++) {
    for(int j=100;j<1000;j++) {
      int prod = i*j;
      if(is_palindromic(prod) && prod > answer)
        answer = prod;
    }
  }
  
  printf("Answer: %d\n", answer);
}

