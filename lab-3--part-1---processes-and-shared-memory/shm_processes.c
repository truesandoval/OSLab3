#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void  ClientProcess(int [], int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;
     int fd[2];
     int buffer[10];
     pipe(fd);

     if (argc != 11) {
          printf("Use: %s #1 #2 #3 #4 #5 #6 #7 #8 #9 #10\n", argv[0]);
          exit(1);
     }

     ShmID = shmget(IPC_PRIVATE, 10*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     printf("Server has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1) {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     printf("Server has attached the shared memory...\n");
    int i;
    for (i = 0; i < 10; i++){
      ShmPTR[i] = atoi(argv[i+1]);
    }

     printf("Server is about to fork a child process...\n");
     pid = fork();
     if (pid < 0) {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     else if (pid == 0) {
          ClientProcess(ShmPTR, fd);
          exit(0);
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
     close(fd[1]);
  
     printf("Parent: ");
     for (i = 0; i < 10; i++){
       read (fd[0], buffer, sizeof(int));
       printf("%d ", *buffer);
     }
     printf("\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}

void  ClientProcess(int  SharedMem[], int fd[])
{
  
     int c, p;
  
     printf("   Client process started\n");
     printf("Child: ");
     for(c = 0; c < 10; c++){
       printf("%d ", SharedMem[c]);
     }
     printf("\n");
     close(fd[0]);
      
     printf("Parent: ");
     for(p = 0; p < 10; p++){
       write(fd[1], &SharedMem[p], sizeof(int)); 
     }
     printf("Done Writing to pipe\n");
     printf("Client is about to exit\n");
}
