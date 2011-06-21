#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <stdint.h>
#define MAP_SIZE 15
using namespace std;

struct position { int col; int row; };
struct location { int col; int row; float cost; };
vector<float> answers;
list<location> olist;
list<location> clist;
int matrix[MAP_SIZE][MAP_SIZE];


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
      split(line, ' ', values);
      for(int i=0;i<values.size();i++) {
        matrix[i][j] = atoi(values[i].c_str());
      }
      j++;
    }
  }
}

bool compare_locations(location a, location b) {
  if(a.cost == b.cost && a.row < b.row) return true;
  if(a.cost/((MAP_SIZE-a.row)) > b.cost/((MAP_SIZE-b.row))) return true;
  return false;
}

void print_options(location ret) {
  printf("[ ");
  for(list<location>::iterator it = olist.begin(); it != olist.end(); it++) {
    printf("(%d,%d %f) ", it->col, it->row, it->cost/(MAP_SIZE-it->row));
  }
  printf("]\n");
}

/*
 * Will return true and update the cost if this node was found on the open list
 */
bool open_list(vector<location> &new_locations, int pos) 
{
  for(list<location>::iterator it = olist.begin(); it != olist.end(); it++) {
    if(it->row == new_locations[pos].row && 
       it->col == new_locations[pos].col) {
      if (it->cost < new_locations[pos].cost) {
        it->cost = new_locations[pos].cost;
      }
      return true;
    }
  }
  return false; 
}

/*
 * Will return true if this node was found on the closed list
 */
bool closed_list(vector<location> &new_locations, int pos)
{
  for(list<location>::iterator it = clist.begin(); it != clist.end(); it++) {
    if(it->row == new_locations[pos].row &&
       it->col == new_locations[pos].col) {
      return true;
    }
  }
  return false;
}

location best_option(vector<location> &new_locations)
{
  for(int i=0;i<new_locations.size();i++) {
    if(!olist.size()) {
      olist.push_back(new_locations[i]);
    } else {
      /* 
       * If it is currently in the open list, update cost
       */
      if(!open_list(new_locations, i)) { 
        olist.push_back(new_locations[i]);
      }
    }
  }
  olist.sort(compare_locations);
  location ret = olist.front();
  clist.push_back(ret);
  if(olist.size())
  olist.pop_front();
  return ret;
}

static inline position add_path(location current) {
  position new_pos;
  new_pos.col = current.col;
  new_pos.row = current.row;
  return new_pos;
}

static inline location right(location current) {
  location right;
  right.col = current.col;
  right.row = current.row-1;
  right.cost = current.cost + matrix[right.col][right.row];
  return right;
}

static inline location left(location current) {
  location left;
  left.col = current.col-1;
  left.row = current.row-1;
  left.cost = current.cost + matrix[left.col][left.row];
  return left;
}

location a_star(location start) {
  location current = start;
  vector<location> discovered;
  while(true) {
    if (current.row == 0) return current;
    if(current.col == 0) {
      // only evaluate the right entry
      discovered.push_back(right(current));
    } else if (current.col == current.row) {
      // only evalute the left entry
      discovered.push_back(left(current));
    } else {
      // evaluate both right and left etnries
      discovered.push_back(right(current));
      discovered.push_back(left(current));
    }
    current = best_option(discovered);
    if(current.row == 0) {
      return current;
    }
    discovered.clear();
  }
}

location start_a_star() {
  vector<location> init;
  
  for(int i=0;i<MAP_SIZE;i++) {
    location added;
    added.row = MAP_SIZE-1;
    added.col = i;
    added.cost = matrix[i][MAP_SIZE-1];
    init.push_back(added);
  }
  location start = best_option(init); 
  return a_star(start);

}


int main(int argc, char*argv[]) {
  printf("Reading file triangle.txt\n");
  read_file("triangle.txt");
  location answer = start_a_star();
  printf("\n\nFinal Answer:\n----------------------------------------------------\n");
  printf("Total cost: %f\n", answer.cost);
  printf("\n");
}
