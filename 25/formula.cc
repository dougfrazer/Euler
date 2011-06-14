#include <stdio.h>

int main(int argc, char*argv[]) {
  int digits = 20;
  int calc_f = 1;
  digits -= 1;
  calc_f += ((digits)/4)*19;
  if((digits)%4 == 1) calc_f += 5;
  else if ((digits)%4 == 2) calc_f += 10;
  else if ((digits)%4 == 3) calc_f += 15;

  calc_f+=1; // move it just above the threshold
  
  printf("First Fibonacci number with %d digits is F_%d\n", digits+1, calc_f);
}
