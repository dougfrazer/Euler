#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define MAX_PRIME 1000000
#define true 1
#define false 0

/* === code-copy from 50.c == */
static unsigned int primes[MAX_PRIME / 2];

char * cachefilename( char ** str )
{
char * home = getenv("HOME");
char * filename = ".primecache";
int size = strlen(home) + strlen(filename) + 2;
*str = (char*)calloc( size, sizeof(char) );
strcpy( *str, home );
strcat( *str, "/" );
strcat( *str, filename );
return *str;
}

static int is_prime( unsigned int x )
{
if ( x == 1 ) return false;
else if ( x < 4 ) return true;
else if ( x % 2 == 0 ) return false;
else if ( x < 9 ) return true;
else if ( x % 3 == 0 ) return false;
else
{
double r;
unsigned int s;
r = (int) sqrt((double) x);
for( s = 5; s <= r; s += 6 )
{
if ( x % s == 0 ) return false;
if ( x % (s + 2) == 0 ) return false;
}
}
return true;
}

static void gen_primes(int n)
{
int fp;
unsigned int i = 2, index = 0, bytes;
char * filename;
fp = open(cachefilename( &filename ), O_RDWR | O_CREAT, S_IRWXU);

if ( fp == -1 )
perror("open");

while ( i < n)
{
bytes = read( fp, &i, sizeof(unsigned int) );

if ( bytes == 0 ) break;
if ( bytes == -1 ) perror("read");

primes[index++] = i;
}

for ( ; i < n; i++ )
{
if ( is_prime(i) )
{
write( fp, &i, sizeof(unsigned int) );
primes[index++] = i;
}
}

close(fp);
}

/* end code-copy */

int main( int argc, char * argv[] )
{
int i;
gen_primes(MAX_PRIME);
printf("%u\n", primes[10001 - 1]);
return 0;
}
