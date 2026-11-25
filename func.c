#include <stdio.h>
int foo(void);

int main(){
printf("%zu\n",sizeof(int));
printf("%zu\n",sizeof(float));
printf("%zu\n",sizeof(double));
printf("%zu",sizeof(long double));


int i;
i = foo();
printf("%d\n",i);

}
int foo(void){
return 3490;
}

