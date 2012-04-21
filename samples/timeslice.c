#include <stdio.h>
#include <sched.h>

int main(int argc,char* argv[]){
  struct timespec tp;
  int status;
  status  = sched_rr_get_interval(0, &tp);
  if( status == 0 )
    printf("Timeslice is %d nano secs\n",tp.tv_nsec);
  else
    printf("fail status is %d",status);
}

