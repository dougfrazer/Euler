int main(int argc, char*argv[]) {
  int blah =0;
  for(int i=0;i<1000000000;i++) {
    if(i&1){blah++;}
  }
}
