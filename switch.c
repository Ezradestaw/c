#include <stdio.h>

int main(void){
int goat_count = 0;
switch(goat_count){
case 0:
	printf("you have no goats");
	[[fallthrough]];
case 1:
	printf("\nyou have a singular goat.\n");
	break;
case 2:
	printf("you have brace of goats.\n");
	break;
default:
	printf("you have a bona fide plethora of goats!\n");
	break;
}
}
