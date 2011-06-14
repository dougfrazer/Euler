#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <functional>
#include <stdlib.h>
#include <list>
#include <stdint.h>
#define MAP_SIZE 80
using namespace std;

struct location { int x; int y; uint64_t cost; };
int matrix[MAP_SIZE][MAP_SIZE];
list<location> olist;


void split(const string &s, char c, vector<string> &v) {
  string::size_type i = 0;
  string::size_type j = s.find(c);
  while(j != string::npos) {
    v.push_back(s.substr(i, j-i));
    i = ++j;
    j = s.find(c,j);
    if (j==string::npos)
      v.push_back(s.substr(i, s.length()));
  }
}

void read_file(const char *file_name) {
  ifstream myReadFile;
  myReadFile.open(file_name);
  char line[512];
  if(myReadFile.is_open()) {
    int j = 0;
    while(!myReadFile.eof()) {
      myReadFile.getline(line, 512);
      vector<string> values;
      split(line, ',', values);
      for(int i=0;i<values.size();i++) {
        matrix[j][i] = atoi(values[i].c_str());
      }
      j++;
    }
  }
}

bool compare_locations(location a, location b) {
  if(a.cost < b.cost) return true;
  return false;
}

void print_options(location ret) {
  for(list<location>::iterator it = olist.begin(); it != olist.end(); it++) {
    printf("(%d,%d %lu)  ", it->x, it->y, it->cost);
  }
  printf("]\n", ret.x, ret.y, ret.cost);
}

location best_option(vector<location> &new_locations) {
  bool skip = false;
  for(int i=0;i<new_locations.size();i++) {
    if(!olist.size()) {
      olist.push_back(new_locations[i]);
    } else {
      for(list<location>::iterator it = olist.begin(); it != olist.end(); it++) {
        if(it->x == new_locations[i].x && 
           it->y == new_locations[i].y) {
          if (it->cost > new_locations[i].cost) {
            it->cost = new_locations[i].cost;
          }
          skip = true;
          continue;
        }
      }
      if(!skip) olist.push_back(new_locations[i]);
      skip = false;
    }
  }
  olist.sort(compare_locations);
  /*
  for(list<location>::iterator it=olist.begin(); it!=olist.end(); it++) {
    if(it->x == MAP_SIZE - 1 && it->y == MAP_SIZE - 1) {
      return *it;
    }
  }
  */
  location ret = olist.front();
//  print_options(ret);
  if(olist.size())
  olist.pop_front();
//  print_options(ret);
  return ret;
}

uint64_t a_star() {
  location current = { 0,0,matrix[0][0] };
  vector<location> discovered;
  while(true) {
    if(current.y < MAP_SIZE) {
      location down = { 
        current.x, 
        current.y+1, 
        current.cost + matrix[current.x][current.y+1] 
      };
      discovered.push_back(down);
    }
    if(current.x < MAP_SIZE) {
      location left = { 
        current.x+1, 
        current.y, 
        current.cost + matrix[current.x+1][current.y] };
      discovered.push_back(left);
    }
    current = best_option(discovered);
    if(current.x == MAP_SIZE - 1 && current.y == MAP_SIZE - 1) return current.cost;
    discovered.clear();
  }
}

int main(int argc, char*argv[]) {
  read_file("matrix.txt");
  printf("Answer: %lu\n", a_star());
}
