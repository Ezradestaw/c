#include <stdio.h>
enum Day{MONDAY,TUESDAY,WEDNESDAY,THURSDAY,FRIDAY,SATURDAY,SUNDAY};
int main(void){
enum Day today = WEDNESDAY;
switch (today){
case MONDAY:
	printf(",Today is Monday");
	break;
case TUESDAY:
	printf("Tuesday");
	break;
case WEDNESDAY:
	printf("congratulations today is Wednesday!!");
	break;
default: 
	printf("I do not care about anything");
}
}
