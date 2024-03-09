#include <stdio.h>

int main(){
	int a=0;
	int *b=&a;

	int c = *b==&a;
	printf("%d\n",c);
	return 0;
}
