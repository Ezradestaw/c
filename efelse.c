#include <stdio.h>

int main(void){
int goat_count = 0;
if(goat_count == 0)
	printf("you have no goats.\n");
else if (goat_count == 1)
	printf("you have a singular goat.\n");
else if (goat_count == 2)
	printf("you have a brace of goats.\n");
else
	printf("you have a bona fide plethora of goats!");
}
