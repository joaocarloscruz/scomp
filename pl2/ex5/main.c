#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>

int main(){
    pid_t v_pid[N]={0};
    for (int i = 0; i< argc; i++){
        if (v_pid[i]==0){
            v_pid[i]= fork();
            if(v_pid[i]<0){
                error();
                exit(0);
            }
            if(v_pid[i]==0){
                exec(argv[i+1], argv[i+1]);
            }else{
                kill(v_pid[i], SIGCONT);
            }
            sleep(5);
            kill(v_pid[i], SIGSTOP);
        }
    }
    return 0;
}