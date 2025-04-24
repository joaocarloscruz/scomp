#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>
#include<string.h>

void handle_stop(int signo){
   printf("[Child %d] Suspending...\n", getpid());
   pause();
}

void handle_cont(int signo){
   printf("[Child %d] Resuming...\n", getpid());
}

int main(int argc, char* argv[]){
   int num_children = argc - 1;
   pid_t child_pid[num_children];
   struct sigaction act1, act2;

   memset(&act1, 0, sizeof(act1));
   act1.sa_handler = handle_stop;
   act1.sa_flags = SA_RESTART;
   memset(&act2, 0, sizeof(act2));
   act2.sa_handler = handle_cont;
   act2.sa_flags = SA_RESTART;

   sigaction(SIGSTOP,&act1,NULL);
   sigaction(SIGCONT,&act2,NULL);
      
   // create children
   for(int i = 0; i < num_children; i++){
      child_pid[i] = fork();
      if(child_pid[i] < 0){
         perror("fork failed");
         exit(EXIT_FAILURE);
      }

      // parent sends a stop signal to every child in the beginning.
      if(child_pid[i] > 0){
         kill(child_pid[i], SIGSTOP);
      }
      if(child_pid[i] == 0){
         char* const arg[] ={argv[i + 1], NULL};
         if(execvp(arg[0],arg) < 0){
            perror("exec failed");
            exit(EXIT_FAILURE);
         }
         
      }
   }
   int child = 0;
   pid_t term_pid;
   int active_children = num_children;
   while(active_children > 0){     
      kill(child_pid[child], SIGCONT);
      sleep(5);
      kill(child_pid[child], SIGSTOP);
      if((term_pid = waitpid(-1, NULL, WNOHANG)) > 0){
         active_children--;
      }
      child++;
      if(child == num_children){
         child = 0;
      }
   }

   printf("All programs have been executed\n");


   return 0;
}
