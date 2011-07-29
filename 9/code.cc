#include <stdlib.h>
#include <stdio.h>


int main (int argc, char*argv[]) {
	for(int i=1;i<1000;i++) {
		for (int j=1;j<1000;j++) {
			for (int k=1;k<1000;k++) {
				if(i*i + j*j == k*k && i+j+k==1000) {
					printf("Answer is (%d,%d,%d) = %d\n", i,j,k,i*j*k);
					return (0);
				}
			}
		}
	}
}
