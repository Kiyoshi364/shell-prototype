#include  <stdio.h>
#include  <signal.h>
#include  <stdlib.h>

void handler(int);

int  main(void){

        signal(SIGTSTP, handler);
        while (1)
        pause();
        return 0;
}

void  handler(int sig){

        signal(sig, SIGTSTP);
        printf(" você apertou ctrl Z\n");
        signal(SIGTSTP, handler);
         
}  
