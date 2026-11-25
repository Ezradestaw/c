#include <stdio.h>

int main(void){
int y=0;int x = 23;
y +=x>10? 17:37;
printf("%d",y);
//returhhn 0;
printf("\nequivalent conditions\n");
y = 0;
if (x>10)
y+=17;
else
y+=37;
printf("%d",y);
return 0;
}
