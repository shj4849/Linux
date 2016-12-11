#include <stdio.h>

int main(){
	char a[20];
	scanf("%s",a);
	if(-1 == remove(a)){
		printf("file remove fail");
	}
}
