#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/mman.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/queue.h>
#include <time.h>

#define NUMTHREADS 18

void client (int *);
extern void init(void);
extern void RankedQuery(char *, int);

extern int passiveTCP(const char *, int);


fd_set fds;
char *mem;

static const char fs[2][20] = {"/dev/rda1h","/dev/rda0h"};

void main (void) {
  pthread_t cthread[NUMTHREADS];
  int i;
  int fd[NUMTHREADS][2];
  int msock, ssock;
  struct sockaddr from;
  int fromlen;
  int numfds;
  int j;
  char buf[1024];

  FD_ZERO(&fds);
  numfds = getdtablesize();
  printf("numfds: %d\n", numfds);

  init();

/*   for (i=0; i<NUMTHREADS; i++) { */
/*     if (pipe(fd[i]) < 0) { */
/*       perror("pipe"); */
/*       exit(1); */
/*     } */
/*     pthread_create(&cthread[i], NULL, (void *)client, (void *)&fd[i]); */
/*   } */


  msock = passiveTCP("5000", 40);
  
  for (j=0; j<2000; j++) {
    ssock = accept(msock, &from, &fromlen);
    bzero(buf, 1024);
    read(ssock, buf, sizeof(buf));    
    printf(".");
    fflush(stdout);
    RankedQuery(buf, ssock);
    close(ssock);   


/*     if (select(numfds, NULL, &fds, NULL, 0) < 0) { */
/*       perror("select"); */
/*       exit(1); */
/*     } */

/*     for (i=0; i < NUMTHREADS; i++) */
/*       if (FD_ISSET(fd[i][1], &fds)) break; */

/*     printf("Master calling on fd: %d\n", fd[i][1]); */
/*     FD_CLR(fd[i][1], &fds); */
/*     write(fd[i][1], &ssock, sizeof(int)); */
  }


  //  pthread_join(cthread[0], NULL); 

 
}

void client (int *fd) {
  int ssock;
  char buf[2048];

  printf("Client listening on fd: %d\n", fd[0]);

  while (1) {
    FD_SET(fd[1], &fds);
    read(fd[0], &ssock, sizeof(int));    
    
    read(ssock, buf, sizeof(buf));

    RankedQuery(buf, ssock);

    close(ssock);

  }
}


