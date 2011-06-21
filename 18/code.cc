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
struct location { int col; int row; int cost; vector<position> path; };
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
  if(a.cost/(MAP_SIZE-a.row) > b.cost/(MAP_SIZE-b.row)) return true;
  if(a.cost == b.cost && a.row < b.row) return true;
  return false;
}

void print_options(location ret) {
  printf("[ ");
  for(list<location>::iterator it = olist.begin(); it != olist.end(); it++) {
    printf("(%d,%d %d) ", it->col, it->row, it->cost);
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
        printf("Updating cost of (%d,%d) from %d to %d\n", 
            new_locations[pos].col, new_locations[pos].row, it->cost, new_locations[pos].cost);
        it->cost = new_locations[pos].cost;
      }
      printf("Skipping (%d,%d) without updating cost\n", 
          new_locations[pos].col, new_locations[pos].row);
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
      printf("Skipping (%d,%d) because its in the closed list\n", 
        new_locations[pos].col, new_locations[pos].row);
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
       * If it is currently in the closed list, skip 
       */
      if(!open_list(new_locations, i) && !closed_list(new_locations, i)) {
        olist.push_back(new_locations[i]);
      }
    }
  }
  olist.sort(compare_locations);
  location ret = olist.front();
  clist.push_back(ret);
  //print_options(ret);
  if(olist.size())
  olist.pop_front();
  //print_options(ret);
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
  right.path.push_back(add_path(current));
  return right;
}

static inline location left(location current) {
  location left;
  position curr_path = { current.col, current.row };
  left.col = current.col-1;
  left.row = current.row-1;
  left.cost = current.cost + matrix[left.col][left.row];
  left.path.push_back(add_path(current));
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
    if(current.row == 0) return current;
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
  position curr_path;
  curr_path.row = start.row;
  curr_path.col = start.col;
  start.path.push_back(curr_path);
  return a_star(start);

}


int main(int argc, char*argv[]) {
  read_file("triangle.txt");
  for(int row=0;row<MAP_SIZE;row++) {
    for(int col=0;col<MAP_SIZE;col++) {
      if(matrix[col][row] != 0) printf("%02d ", matrix[col][row]);
    }
    printf("\n");
  }
  location answer = start_a_star();
  printf("\n\nFinal Answer:\n----------------------------------------------------\n");
  for(int i=0;i<MAP_SIZE-1;i++) {
    printf("(%d,%d)->", answer.path[i].col, answer.path[i].row);
  }
  printf("(%d,%d) \nTotal cost: %d\n", answer.col, answer.row, answer.cost);
}
