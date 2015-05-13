/**
    lfsr_reverse.c
    LFSR reverse algorithm reverse by Brute Force

    => objective is to find 2 integers value (32bitsx2) from a specific LFSR alogrithm
    @author Bertrand Martel
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//number of thread to launch
#define NTHREADS 30

//final iteration must go to ITERATION_MAX
#define ITERATION_MAX 2147483647

//number of iterations to check per thread (except the last which go straight to the end)
#define ITERATION_AVERAGE_PER_THREAD 98426333

//check program time
clock_t begin, end;
double time_spent;

//Function called in each of 30 threads processing the algorithm
void *checkEncodingLfsr(void *x)
{
  //LFSR periods
  int period1 = 0x05 << 8 | 0xC7;
  int period2 = 0x15 << 8 | 0x83;

  //LFSR taps
  int filter = 0xD0000001;

  //to change if needed (expecting result)
  int B2 = 0xD6<<24 | 0x50 << 16 | 0x70 << 8 | 0x38;
  int A2 = 0xDC<<24 | 0x6C << 16 | 0x06 << 8 | 0x7D;

  int threadId;
  threadId = *((int *) x);

  int i;
  // find k
  if (threadId!=29)
  {
    int begin = 0x80000000+ITERATION_AVERAGE_PER_THREAD*threadId;
    int end = 0x80000000+ITERATION_AVERAGE_PER_THREAD*(threadId+1);

    printf("Checking from %d to %d for thread %d ...\n",begin,end,threadId);

    for (i = begin;i< end;i++)
    {
      int result = encoder32bitLFSR(i,period1,filter);
      if (result == B2)
      {
        printf("Found the solution for B => %d\n",i);
        break;
      }
    }
  }
  else
  {
     
    int begin = 0x80000000+ITERATION_AVERAGE_PER_THREAD*threadId;
    int end = ITERATION_MAX;

    printf("Checking from %d to %d for thread %d ...\n",begin,end,threadId);
   
    for (i = begin;i< end;i++)
    {
      int result = encoder32bitLFSR(i,period1,filter);
      if (result == B2)
      {
        printf("Found the solution for B => %d\n",i);
        break;
      }
    }
  }

  //find C
  if (threadId!=29)
  {
  
    int begin = 0x80000000+ITERATION_AVERAGE_PER_THREAD*threadId;
    int end = 0x80000000+ITERATION_AVERAGE_PER_THREAD*(threadId+1);

    printf("Checking from %d to %d for thread %d ...\n",begin,end,threadId);

    for (i = begin;i< end;i++)
    {
      int result = encoder32bitLFSR(i,period2,filter);
      if (result == A2)
      {
        printf("Found the solution for A => %d\n",i);
        return NULL;
      }
    }
  }
  else
  {
    int begin = 0x80000000+ITERATION_AVERAGE_PER_THREAD*threadId;
    int end = ITERATION_MAX;

    printf("Checking from %d to %d for thread %d ...\n",begin,end,threadId);

    for (i = begin;i< end;i++)
    {
      int result = encoder32bitLFSR(i,period2,filter);
      if (result == A2)
      {
        printf("Found the solution for A => %d\n",i);
        return NULL;
      }
    }
  }

  printf("End of thread %d!\n", threadId);
  return NULL;
}

// start Brute Force on LFSR algorithm
int main(int argc, char *argv[])
{
  begin = clock();

  pthread_t threads[NTHREADS];
  int thread_args[NTHREADS];
  int rc, i;
  
  /* spawn the threads */
  
  for (i=0; i<NTHREADS; ++i)
    {
      thread_args[i] = i;
      printf("spawning thread %d\n", i);
      rc = pthread_create(&threads[i], NULL, checkEncodingLfsr, (void *) &thread_args[i]);
    }
  
  /* wait for threads to finish */

  for (i=0; i<NTHREADS; ++i) {
    rc = pthread_join(threads[i], NULL);
  }

  return 1;
}

/**
 * Lfsr 32 bit algorithm (integer)
 * 
 * @param seed
 *    input of lfsr algo
 * @param period
 *    iteration number
 * @param filter
 *    polynomial used
 * @return
 *    lfsr encoding result
 */
int encoder32bitLFSR(int seed,int period,int filter)
{
    int i;
      for (i = 0; i < period; i++) {
          if((seed & 0x80000000) != 0) {
            seed = seed << 1^filter;
          } else {
            seed <<= 1;
          }
      }
      return seed;
}