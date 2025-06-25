#include <stdio.h> 
#include <stdlib.h> 

unsigned long factorial();

int main(){
	int num;
	scanf("%d", &num);

	unsigned long res = factorial(num, 0);
	printf("%lu\n", res);
	fflush(stdout);
	return 0;

}

unsigned long factorial(int num){
	if(num == 0 || num == -1){
		return 1;
	}
	return num * factorial(num - 1); 

	
}
