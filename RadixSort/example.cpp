#include <chrono>
#include <cstdlib>
#include <random>
#include <fstream>
#include "sequenceIO.h"
#include "parseCommandLine.h"
#include "radixSort.h"
#include "ska_sort.hpp"

using namespace std;
using namespace benchIO;

typedef std::chrono::high_resolution_clock high_res_clock;

int f(int x) {
	return x % 1000000;
}
typedef pair<uintT,uintT> uintTPair;
int pairF(uintTPair x) {
	return x.first % 1000000;
}

void check_sorted(uintT*  array, long length) {
  uintT prev = array[0];
  parallel_for (long i=1;i<length;i++) {
    if(f(array[i]) < f(array[i-1])) {
      printf("FAIL: ARRAY NOT SORTED!\n");
      exit(0);
    }
  }
  printf("array is sorted!\n");
}

template <class T>
void check_sorted_pairs(pair<uintT,T>* array, long length) {
  bool sorted = true;
  long stability_error = 0;
  parallel_for (long i=1;i<length;i++) {
    if(pairF(array[i]) < pairF(array[i-1])) {
      sorted = false;
    }

  }
  if(!sorted){
	printf("FAIL: ARRAY NOT SORTED!\n");
  } else {
  printf("array is sorted!\n");
  }
}


int main(int argc, char **argv) {
  commandLine P(argc,argv,"[-r <rounds>] [-c] <inFile>");
  char* iFile = P.getArgument(0);
  int rounds = P.getOptionIntValue("-r",1);
  int workers = P.getOptionIntValue("-w",0); 
  if(workers != 0) {
   	setWorkers(workers);
  }
  bool check = P.getOption("-c");

  cout << "workers = " << getWorkers() << endl;
  seqData D = readSequenceFromFile(iFile);
  elementType dt = D.dt;
  long length = D.n;

  if(dt == intType){ 

    uintT* array = (uintT*) D.A;
    uintT* control_array = newA(uintT,length);
	  


    parallel_for(long i=0;i<length;i++) control_array[i] = array[i];

		parallelIntegerSort(array, length, f);
  
    if(check) {
      check_sorted(array,length);
    }
    free(array); free(control_array);
  } else if (dt == intPairT) {
    uintTPair* array = (uintTPair*) D.A;

    uintTPair* control_array = newA(uintTPair,length);
    parallel_for(long i=0;i<length;i++) control_array[i] = array[i];

		parallelIntegerSort(array, length, pairF);

    if(check) {
      check_sorted_pairs(array,length);
    }
;
    free(array); free(control_array);
  } else if (dt == intStringPairT) {
    cout << "to implement" << endl;
  } else {
    cout << "input file not of right type" << endl;
  }

  return 0;
}

