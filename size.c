#include <stdio.h>

int main(void)
{
int x[12];
printf("%zu\n",sizeof x);
printf("%zu\n",sizeof(int));
printf("%zu/%zu is the size of the array x which is %zu",sizeof(x),sizeof(int),sizeof(x)/sizeof(int));
}
