#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>


int main()
{
    int n=5;
    int status;
    int arr[2000];
    int arr_pid[10];
    
    srand(time(NULL)*getpid());

    for (size_t i = 0; i < 2000; i++)
    {
        arr[i] = (rand() % 20-1)+1;
    }

    
    /*for (size_t i = 400; i < 410; i++)
    {
        printf("%ld: %d\n",i,arr[i]);
    }*/
   


    size_t i = 0;
    size_t limit = 200;
    for (size_t j = 0; j < 10; j++)
    {
        i=200*j;
        limit+= 200*j;
        if ((arr_pid[j]=fork())==0)
        {
            int x =0;
            while(i<limit)
            {
                x++;
                if (n==arr[i])
                {
                    exit(x);
                }
                i++;
            }
            exit(255);  
        }
    }


    for (size_t i = 0; i < sizeof(arr_pid)/sizeof(arr_pid[1]); i++)
    {
        waitpid(arr_pid[i],&status,0);
        if (WEXITSTATUS(status)!=255)
        {
            printf("Posicao: %ld\n", WEXITSTATUS(status)+(200*i)-1);
        }
        
    }
    
    
    
} 


