#include <stdio.h>
#include <stdint.h>

typedef struct { int x; int y; } location;

location end;
uint64_t count = 0;

void go_to_goal(int x, int y) {
  printf("Starting new search from (%d,%d)\n", x,y);
  while(x != end.x && y != end.y) {
    if(y < end.y) go_to_goal(x,y+1);
    if(x == end.x && y == end.y) break;
    else if(x < end.x) x++;
    else if(x==end.x){ x=1;y++;}
  }
  count++;
}


int main(int argc, char*argv[]) {
  int answer = 0;
  end.x = 3; end.y=3;
  go_to_goal(0,0);
  printf("Answer: %lu\n", count);
}

