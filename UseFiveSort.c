/* 
File: c:/bsd/rigel/sort/Licence.txt
Date: Sat Dec 28 15:43:13 2013

Copyright (c) 2013, Dennis de Champeaux.  All rights reserved.

The copyright holders hereby grant to any person obtaining a copy of
this software (the "Software") and/or its associated documentation
files (the Documentation), the irrevocable (except in the case of
breach of this license) no-cost, royalty free, rights to use the
Software for non-commercial evaluation/test purposes only, while
subject to the following conditions:

. Redistributions of the Software in source code must retain the above
copyright notice, this list of conditions and the following
disclaimers.

. Redistributions of the Software in binary form must reproduce the
above copyright notice, this list of conditions and the following
disclaimers in the documentation and/or other materials provided with
the distribution.

. Redistributions of the Documentation must retain the above copyright
notice, this list of conditions and the following disclaimers.

The name of the copyright holder, may not be used to endorse or
promote products derived from this Software or the Documentation
without specific prior written permission of the copyright holder.
 
THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS", WITHOUT WARRANTY
OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION OR THE USE OF OR
OTHER DEALINGS WITH THE SOFTWARE OR DOCUMENTATION.
*/


// File: c:/bsd/rigel/sort/UseFiveSort.c
// Date: Sat Dec 28 15:43:30 2013

/*
   This file is a test bench for excercising fivesort and testing 
   it against other algorithms.
   >>Specific objects and a specific comparison function are used throughout<<.
   Feel free to change them as you see fit.
  
   The comparison function given here does NOT work for qsort, because qsort 
   thinks that entities to be sorted reside in the array, while sixsort knows
   that the array contains pointers to objects.  It >is< possible, of course,
   to construct an alternative comparison function for qsort.

   To compile:  $ gcc UseFiveSort.c FiveSort.o
   It produces the file: a.exe (or the like)


   The main function has many alternative functions.
   Un-comment one to activate.
   Inside such a function are often other choices that can be selected.
 */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

void fivesort(void **AA, int size, 
	int (*compar ) (const void *, const void * ));
void testQuicksort0();
void testFivesort();
void testFiveSort2();
void testBentley();
void timeTest();
void validateXYZ();
void validateBentley();
void callCut2(void **AA, int size, 
	int (*compar ) (const void *, const void * ));
void callQuicksort0(void **AA, int size, int (*compar ) () );
void callBentley(void **A, int N, int M);
void compareQuicksort0AgainstFivesort();
void compareCut2AgainstFivesort();
void compareBentleyAgainstFivesort(); // on Bentley test bench
int clock();
void validateFiveSortBT();

// Example of objects that can be used to populate an array to be sorted:
  // To obtain the int field from X: ((struct intval *) X)->val
  // To obtain the float field from X: ((struct intval *) X)->valf
  struct intval {
    int val;
    float valf;
  };

// Here an exmple comparison function for these objects:
// **** NOTE, again **** 
//      All the examples below use ::::
//          the intval objects, and 
//          the compareIntVal comparison function
int compareIntVal (const void *a, const void *b)
{
  struct intval *pa = (struct intval *) a;
  struct intval *pb = (struct intval *) b;
  return (pa->val - pb->val);
}
// Alternative comparison fuunction used by bentley()
int compareInt(const void **a, const void **b) {
  struct intval *pa = (struct intval *) *a;
  struct intval *pb = (struct intval *) *b;
  return (pa->val - pb->val);
} // end compareInt

int main (int argc, char *argv[]) {
  printf("Running UseFiveSort ...\n");
  // Un-comment one of the following to use a specific functionality.
  // Modify the body of these functions as desired

  // To ask for the license expiration date and the host
     // fivesort(0, 0, 0);
  // To check that fivesort produces a sorted array
  testFivesort();
  // testBentley();
  // Ditto but using the general function testAlgorithm
     // ... and uncomment also testFiveSort2 ...
     // testFiveSort2();
  // Compare the outputs of two sorting algorithms
  validateXYZ(); // must provide another algorithm XYZ
     // ... and uncomment validateXYZ ...
  // validateBentley();
  // Measure the sorting time of an algorithm
  // timeTest();
  // Compare the speed fraction of two algorithms
     // compareFivesortAgainstXYZ();
     // ... and uncomment also compareFivesortAgainstXYZ ...
  // Whatever here:::
  // compareQuicksort0AgainstFivesort();
  // compareCut2AgainstFivesort();    
  // compareBentleyAgainstFivesort(); // on Bentley test bench
  // testQuicksort0();
  // validateFiveSortBT();
  return 0;
} // end of main

void *myMalloc(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    fprintf(stderr, "malloc fails for: %s\n", location);
    exit(1);
  }
  return p;
}

// fillarray assigns random values to the int-field of our objects
void fillarray(void **A, int lng, int startv) {
  const int range = 1024*1024*32;
  int i;
  srand(startv);
  struct intval *pi;
  for ( i = 0; i < lng; i++) {
    pi = (struct intval *)A[i];
    pi->val = rand()%range; 
  }
} // end of fillarray

// Check that adjacent objects in an array are ordered.
// If not, it reports an error 
void check(void **A, int N, int M) {
  int i; int cnt = 0;
  void *x, *y;
  struct intval *pi;
  for (i = N + 1; i <= M; i++) {
    x = A[i-1]; y = A[i];
    if ( compareIntVal(y, x) < 0 ) {
      pi = (struct intval *) y;
      printf("%s %d %s %d %s", "Error at: ", i, 
	     " A[i]: ", pi->val, "\n");
      cnt++;
    }
  }
  printf("check # errors: %d\n", cnt);
} // end check

void check2(void **A, int N, int M) {
  int i; int cnt = 0;
  void *x, *y;
  struct intval *pi;
  for (i = N + 1; i <= M; i++) {
    x = &A[i-1]; y = &A[i];
    if ( compareInt(y, x) < 0 ) {
      pi = (struct intval *) y;
      printf("%s %d %s %d %s", "Error at: ", i, 
	     " A[i]: ", pi->val, "\n");
      cnt++;
    }
  }
  printf("check # errors: %d\n", cnt);
} // end check2

// initializing an array, sort it and check it
void testFivesort() {
  int siz = 1024 * 1024 * 16;
  // int siz = 1024*64;
  printf("Running testFivesort on size %d ...\n", siz);
  // create array
  struct intval *pi;
  void **A = myMalloc("testFivesort 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testFivesort 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 100);
  // sort it
  int compareIntVal();
  fivesort(A, siz, compareIntVal);
  // and check it
  check(A, 0, siz-1);
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
} // end testFivesort()

// testAlgorithm0 is a generalization of testFivesort;
// another algorithm can be plugged in
void testAlgorithm0(char* label, int siz, void (*alg1)() ) {
  printf("%s on size: %d\n", label, siz);
  // create array
  struct intval *pi;
  void **A = myMalloc("testAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 100);
  // sort it
  int compareIntVal();
  (*alg1)(A, siz, compareIntVal);
  // and check it
  check(A, 0, siz-1);
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
} // end testAlgorithm0

// like testAlgorithm0 but the size of array is preset inside testAlgorithm
void testAlgorithm(char* label, void (*alg1)() ) {
  testAlgorithm0(label, 1024*1024, alg1);
} // end testAlgorithm

void testFiveSort2() {
  void fivesort();
  testAlgorithm("Running fivesort ...", fivesort);
} // end testFiveSort2

void testQuicksort0() {
  void callQuicksort0();
  testAlgorithm0("Check quicksort0()", 1024*1024, callQuicksort0);
} // end testQuicksort0()

void testBentley() {
  //void callBentley();
  // testAlgorithm0("Check bentley - callBentley()", 1024*1024, callBentley);
  // testAlgorithm0("Check bentley - callBentley()", 40, callBentley);
  printf("Running testBentley ...\n");
  // int siz = 1024 * 1024;
  int siz = 64;
  // create array
  struct intval *pi;
  void **A = myMalloc("testBentley 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("testBentley 2", sizeof (struct intval));
    A[i] = pi;
  };
  // fill its content
  fillarray(A, siz, 100);
  /*
  for (i = 0; i < siz; i++) {
    pi = A[i]; printf("A[i] i %d pival %d\n", i, pi->val);
  }
  */
  // sort it
  callBentley(A, 0, siz-1);
  /*
  for (i = 0; i < siz; i++) {
    pi = A[i]; printf("A[i] i %d pival %d\n", i, pi->val);
  }
  */

  // and check it
  check2(A, 0, siz-1);
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
} // end testBentley()


// validateAlgorithm0 is used to check algorithm alg2 against a
// trusted algorithm alg1.
// The check consists of making sure that starting from identical
// inputs they produce identical outputs
void validateAlgorithm0(char* label, int siz, void (*alg1)(), void (*alg2)() ) {
  printf("%s on size: %d\n", label, siz);
  // create the input for alg1 ...
  struct intval *pi;
  void **A = myMalloc("validateAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("validateAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  int startv = 102;
  fillarray(A, siz, startv);
  // ... sort it
  // int compareIntVal();
  (*alg1)(A, siz, compareIntVal);

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  // struct intval *pi;
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, startv);
  // ... sort it
  // int compareIntVal();
  (*alg2)(B, siz, compareIntVal);
 
  // check that the two outputs are the same
  int foundError = 0;
  for (i = 0; i < siz; i++)
    // if ( A[i] != B[i] ) {
    if ( compareIntVal(A[i], B[i]) != 0 ) {
      printf("validate error i: %d\n", i);
      foundError = 1;
    }
  if ( !foundError ) 
    printf("NO error found ...\n");
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); 
    };
    free(A);
    for (i = 0; i < siz; i++) {
      free(B[i]); 
    };
    free(B);
} // end validateAlgorithm0

// Like validateAlgorithm0 but with fixed array size
void validateAlgorithm(char* label, void (*alg1)(), void (*alg2)() ) {
  validateAlgorithm0(label, 1024 * 1024 *16, alg1, alg2);
} // end validateAlgorithm

// Example:: replace XYZ by what you want to validate
void validateXYZ() {
  void fivesort(), callQuicksort0();
  validateAlgorithm("Running validate XYZ against trusted callQuicksort0 ...",
		    fivesort, callQuicksort0);
  /*
  void callCut2(), callQuicksort0();
  validateAlgorithm("Running validate XYZ against trusted callQuicksort0 ...",
		    callCut2, callQuicksort0);
  */
} // end validateXYZ

void validateBentley() { // need to change the 1st function
  int siz = 1024 * 1024;
  char* label = "Validate Bentley against Cut2";
  printf("%s on size: %d\n", label, siz);
  // create the input for alg1 ...
  struct intval *pi;
  void **A = myMalloc("validateAlgorithm0 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("validateAlgorithm0 2", sizeof (struct intval));
    A[i] = pi;
  };
  int startv = 102;
  fillarray(A, siz, startv);
  // ... sort it
  // int compareIntVal();
  // (*alg1)(A, siz, compareIntVal);
  callCut2(A, siz, compareIntVal);

  // create the input for alg2 ...
  void **B = myMalloc("validateAlgorithm0 3", sizeof(pi) * siz);
  // struct intval *pi;
  for (i = 0; i < siz; i++) {
    pi =  myMalloc("validateAlgorithm0 4", sizeof (struct intval));
    B[i] = pi;
  };
  fillarray(B, siz, startv);
  // ... sort it
  // int compareIntVal();
  // (*alg2)(B, siz, compareIntVal);
  callBentley(B, 0, siz-1);
 
  // check that the two outputs are the same
  int foundError = 0;
  for (i = 0; i < siz; i++)
    // if ( A[i] != B[i] ) {
    if ( compareIntVal(A[i], B[i]) != 0 ) {
      printf("validate error i: %d\n", i);
      foundError = 1;
    }
  if ( !foundError ) 
    printf("NO error found ...\n");
  // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); 
    };
    free(A);
    for (i = 0; i < siz; i++) {
      free(B[i]); 
    };
    free(B);
} // end validateBentley

// Run an algorithm and report the time used
void timeTest() {
  printf("timeTest() of fivesort \n");
  int algTime, T;
  int seed;
  int seedLimit = 10;
  int z;
  // int siz = 1024 * 1024 * 16;
int siz = 1024 * 1024*16;
  // construct array
  struct intval *pi;
  void **A = myMalloc("timeTest 1", sizeof(pi) * siz);
  int i;
  for (i = 0; i < siz; i++) {
    pi = myMalloc("timeTest 2", sizeof (struct intval));
    A[i] = pi;
  };

  // warm up the process
  fillarray(A, siz, 666); 
  int sumTimes = 0;
  for (z = 0; z < 3; z++) { // repeat to check stability
    algTime = 0;
    // measure the array fill time
    int TFill = clock();
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
      // here alternative ways to fill the array
      // int k;
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
    TFill = clock() - TFill;
    // now we know how much time it takes to fill the array
    // measure the time to fill & sort the array
    T = clock();
    for (seed = 0; seed < seedLimit; seed++) { 
      fillarray(A, siz, seed);
      // for ( k = 0; k < siz; k++ ) A[k] = 0;
      // for ( k = 0; k < siz; k++ ) A[k] = k%5;
      // for ( k = 0; k < siz; k++ ) A[k] = siz-k;
      fivesort(A, siz, compareIntVal);  
      // callCut2(A, siz, compareIntVal);
    }
    // ... and subtract the fill time to obtain the sort time
    algTime = clock() - T - TFill;
    printf("algTime: %d \n", algTime);
    sumTimes = sumTimes + algTime;
  }
  printf("%s %d %s", "sumTimes: ", sumTimes, "\n");
  // free array
  for (i = 0; i < siz; i++) {
    free(A[i]); 
  };
  free(A);
} // end timeTest

void add(int z, int t, int a[]) {
  if ( z <= 0 ) { a[0] = t; return; }
  if ( a[z-1] <= t ) { a[z] = t; return; }
  a[z] = a[z-1];
  add(z-1, t, a);   
} // end add

// Report the speed fraction of two algorithms on a range of array sizes
void compareAlgorithms0(char *label, int siz, int seedLimit, void (*alg1)(), void (*alg2)() ) {
  printf("%s on size: %d seedLimit: %d\n", label, siz, seedLimit);
  int alg1Time, alg2Time, T;
  int seed;
  int z;
  int limit = 1024 * 1024 * 16 + 1;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    struct intval *pi;
    void **A = myMalloc("compareAlgorithms0 1", sizeof(pi) * siz);
    // construct array
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareAlgorithms0 2", sizeof (struct intval));
      A[i] = pi;
    };
    // warm up the process
    for (seed = 0; seed < seedLimit; seed++) 
      fillarray(A, siz, seed);
    int zLimit = 6; // even
    int alg1Times[zLimit-1];
    int alg2Times[zLimit-1];
    for (z = 0; z < zLimit; z++) { // repeat to check stability
      alg1Time = 0; alg2Time = 0;
      int TFill = clock();
      for (seed = 0; seed < seedLimit; seed++) 
	fillarray(A, siz, seed);
      TFill = clock() - TFill;
      T = clock();
      for (seed = 0; seed < seedLimit; seed++) { 
	fillarray(A, siz, seed);
	(*alg1)(A, siz, compareIntVal); 
      }
      alg1Time = clock() - T - TFill;
      if ( 0 < z ) add(z-1, alg1Time, alg1Times);
      T = clock();
      for (seed = 0; seed < seedLimit; seed++) { 
	fillarray(A, siz, seed);
	(*alg2)(A, siz, compareIntVal);
      }
      alg2Time = clock() - T - TFill;
      if ( 0 < z ) add(z-1, alg2Time, alg2Times);
      printf("%s %d %s", "siz: ", siz, " ");
      printf("%s %d %s", "alg1Time: ", alg1Time, " ");
      printf("%s %d %s", "alg2Time: ", alg2Time, " ");
      float frac = 0;
      if ( alg1Time != 0 ) frac = alg2Time / ( 1.0 * alg1Time );
      printf("%s %f %s", "frac: ", frac, "\n");
    }
    int middle = zLimit/2 - 1;
    int alg1Middle = alg1Times[middle];
    int alg2Middle = alg2Times[middle];
    printf("alg1Middle %d alg2Middle %d\n", alg1Middle, alg2Middle);
    float fracm = 0;
    if ( alg1Middle != 0 ) fracm = alg2Middle/ (1.0 * alg1Middle);
    printf("siz: %d ratio: %f\n", siz, fracm);

    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]);
    };
    free(A);
    // siz = siz * 2;
    // seedLimit = seedLimit / 2;
    siz = siz * 4;
    seedLimit = seedLimit / 4;
  }
} // end compareAlgorithms0

void compareAlgorithms(char *label, void (*alg1)(), void (*alg2)() ) {
  compareAlgorithms0(label, 1024, 32 * 1024, alg1, alg2);
  // compareAlgorithms0(label, 1024*1024, 32, alg1, alg2);
  // compareAlgorithms0(label, 16 * 1024 * 1024, 2, alg1, alg2);
} // end compareAlgorithms

/* Example, replace XYZ by what you want to compare against
compareFivesortAgainstXYZ() {
  void fivesort(), XYZ();
  compareAlgorithms("Compare fivesort vs XYZ", fivesort, XYZ);
}
*/

void compareQuicksort0AgainstFivesort() {
  void fivesort(), callQuicksort0();
  compareAlgorithms("Compare quicksort0 vs fivesort", callQuicksort0, fivesort);
} // end compareQuiksort0AgainstFivesort

void compareCut2AgainstFivesort() {
  void fivesort(), callCut2();
  compareAlgorithms("Compare cut2 vs fivesort", callCut2, fivesort);
} // end compareCut2AgainstFivesort

// void **A;
// int (*compareXY)();
// const int cut2Limit = 127;

void heapc();
void quicksort0();
void quicksort0c();
void iswap();
void dflgm();
void cut2();

void callQuicksort0(void **A, int size, 
	int (*compar ) (const void *, const void * ) ) {
  quicksort0(A, 0, size-1, compar);
} // end callQuicksort0

// void cut2(int N, int M);
void callCut2(void **A, int size, 
	int (*compar ) (const void *, const void * ) ) {
  cut2(A, 0, size-1, compar);
} // end callCut2

void bentley();
// Wrapper function to invoke bentley
void callBentley(void **A, int N, int M) {
  struct intval *pi;
  bentley(&A[N], M-N+1, sizeof(pi), compareInt);
} // end callBentley

// #include <_ansi.h>
#include <stdlib.h>

#ifndef __GNUC__
#define inline
#endif

// static inline char	*med33 _PARAMS((char *, char *, char *, int (*)()));
// static inline void	 swapfunc _PARAMS((char *, char *, int, int));

#define min(a, b)	(a) < (b) ? a : b

/*
 * Qsort routine from Bentley & McIlroy's "Engineering a Sort Function".
 */
#define swapcode(TYPE, parmi, parmj, n) { 		\
	long i = (n) / sizeof (TYPE); 			\
	register TYPE *pi = (TYPE *) (parmi); 		\
	register TYPE *pj = (TYPE *) (parmj); 		\
	do { 						\
		register TYPE	t = *pi;		\
		*pi++ = *pj;				\
		*pj++ = t;				\
        } while (--i > 0);				\
}

#define SWAPINIT(a, es) swaptype = ((char *)a - (char *)0) % sizeof(long) || \
	es % sizeof(long) ? 2 : es == sizeof(long)? 0 : 1;

static inline void swapfunc(a, b, n, swaptype)
     char *a;
     char *b;
     int n;
     int swaptype;
{
	if (swaptype <= 1) 
		swapcode(long, a, b, n)
	else
		swapcode(char, a, b, n)
}

#define swap(a, b)					\
	if (swaptype == 0) {				\
		long t = *(long *)(a);			\
		*(long *)(a) = *(long *)(b);		\
		*(long *)(b) = t;			\
	} else						\
		swapfunc(a, b, es, swaptype)

#define vecswap(a, b, n) 	if ((n) > 0) swapfunc(a, b, n, swaptype)

static inline char * med33(a, b, c, cmp)
     char *a;
     char *b;
     char *c;
     int (*cmp)(); 
{
	return cmp(a, b) < 0 ?
	       (cmp(b, c) < 0 ? b : (cmp(a, c) < 0 ? c : a ))
              :(cmp(b, c) > 0 ? b : (cmp(a, c) < 0 ? a : c ));
}

void bentley(a, n, es, cmp)
     void *a;
     size_t n;
     size_t es;
     int (*cmp)();
{
  char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
  int d, r, swaptype; 
  // The use of swap_cnt and the 2nd invocation of insertionsort has been removed
  // int swap_cnt; 

loop:	SWAPINIT(a, es);
  // swap_cnt = 0;
	if (n < 7) {
		for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
			for (pl = pm; pl > (char *) a && cmp(pl - es, pl) > 0;
			     pl -= es)
				swap(pl, pl - es);
		return;
	}
	pm = (char *) a + (n / 2) * es;
	if (n > 7) {
		pl = a;
		pn = (char *) a + (n - 1) * es;
		if (n > 40) {
			d = (n / 8) * es;
			pl = med33(pl, pl + d, pl + 2 * d, cmp);
			pm = med33(pm - d, pm, pm + d, cmp);
			pn = med33(pn - 2 * d, pn - d, pn, cmp);
		}
		pm = med33(pl, pm, pn, cmp);
	}
	swap(a, pm);
	pa = pb = (char *) a + es;

	pc = pd = (char *) a + (n - 1) * es;
	for (;;) {
		while (pb <= pc && (r = cmp(pb, a)) <= 0) {
			if (r == 0) {
			        // swap_cnt = 1;
				swap(pa, pb);
				pa += es;
			}
			pb += es;
		}
		while (pb <= pc && (r = cmp(pc, a)) >= 0) {
			if (r == 0) {
			        // swap_cnt = 1;
				swap(pc, pd);
				pd -= es;
			}
			pc -= es;
		}
		if (pb > pc)
			break;
		swap(pb, pc);
		// swap_cnt = 1;
		pb += es;
		pc -= es;
	}
	/*
	if (swap_cnt == 0) {  // Switch to insertion sort 
		for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
			for (pl = pm; pl > (char *) a && cmp(pl - es, pl) > 0; 
			     pl -= es)
				swap(pl, pl - es);
		return;
	}
	*/

	pn = (char *) a + n * es;
	r = min(pa - (char *)a, pb - pa);
	vecswap(a, pb - r, r);
	r = min(pd - pc, pn - pd - es);
	vecswap(pb, pn - r, r);
	/* Ordering on the recursive calls has been added to obtain at most 
	   log2(N) stack space 
	if ((r = pb - pa) > es)
		bentley(a, r / es, es, cmp);
	if ((r = pd - pc) > es) { 
	        // Iterate rather than recurse to save stack space 
		a = pn - r;
		n = r / es;
		goto loop;
	}
	*/
	
    int left =  pb - pa;
    int right = pd - pc;
    if ( left <= right ) {
       if ( left > es ) bentley(a, left / es, es, cmp);
       if ( right > es ) {
	   // Iterate rather than recurse to save stack space 
	   a = pn - right;
	   n = right / es;
	   goto loop;
       }
    } else {
       if ( right > es ) bentley(pn-right, right / es, es, cmp);
       if ( left > es ) {
	   // Iterate rather than recurse to save stack space 
	   // a = pn - left;
	   n = left / es;
	   goto loop;
       }
    }
} // end of bentley

// Bentley test-bench content generators
/*
void reverse();
void reverseFront();
void reverseBack();
void tweakSort();
void dither();
*/

void reverse2();
void reverse(void **A, int n) {
  reverse2(A, 0, n-1);
} // end reverse
void reverse2(void **A, int start, int end) {
  void *x;
  while ( start < end ) {
    x = A[start]; A[start++] = A[end]; A[end--] = x;
  }    
} // end reverse2
void reverseFront(void **A, int n) {
  reverse2(A, 0, n/2);
} // end reverseFront
void reverseBack(void **A, int n) {
  reverse2(A, n/2, n-1);
} // end reverseBack

void tweakSort(void **AA, int n) {
  /*
  A = AA;
  compareXY = compareIntVal;
  cut2(0, n-1);
  */
  callCut2(AA, n, compareIntVal);
} // end tweakSort
void dither(void **A, int n) {
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = A[k] + (k % 5);
    pi = (struct intval *)A[k];
    pi->val = pi->val + (k % 5);
  }
} // end dither

void sawtooth(void **A, int n, int m, int tweak) {
  // int *A = malloc (sizeof(int) * n);
  struct intval *pi;
  int k;
  for (k = 0; k < n; k++) {
    pi = (struct intval *)A[k];
    pi->val = k % m; 
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end sawtooth

void rand2(void **A, int n, int m, int tweak, int seed) {
  srand(seed);
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = rand() % m;
    pi = (struct intval *)A[k];
    pi->val = rand() % m;
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end rand2

void stagger(void **A, int n, int m, int tweak) {
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = (k*m+k) % n;
    pi = (struct intval *)A[k];
    pi->val = (k*m+k) % n;
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end stagger

void plateau(void **A, int n, int m, int tweak) {
  int k;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    // A[k] = ( k <= m ? k : m );
    pi = (struct intval *)A[k];
    pi->val = ( k <= m ? k : m );
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end plateau

void shuffle(void **A, int n, int m, int tweak, int seed) {
  srand(seed);
  int k, i, j;
  i = j = 0;
  struct intval *pi;
  for (k = 0; k < n; k++) {
    pi = (struct intval *)A[k];
    if ( 0 == ( rand() %m ) ) { 
      // j = j+2; A[k] = j;
      j = j+2;  pi->val = j;
    } else {
      // i = i+2; A[k] = i;
      i = i+2;  pi->val = i;
    }
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end shuffle

void slopes(void **A, int n, int m, int tweak) {
  int k, i, b, ak;
  i = k = b = 0; ak = 1;
  struct intval *pi;
  while ( k < n ) {
    if (1000000 < ak) ak = k; else
    if (ak < -1000000) ak = -k;
    // A[k] = -(ak + b); ak = A[k];
    pi = (struct intval *)A[k];
    ak = -(ak + b);
    pi->val = ak;
    k++; i++; b++;
    if ( 11 == b ) { b = 0; }
    if ( m == i ) { ak = ak*2; i = 0; }
  }
  if ( tweak <= 0 ) return;
  if ( tweak == 1 ) { reverse(A, n); return; }
  if ( tweak == 2 ) { reverseFront(A, n); return; }
  if ( tweak == 3 ) { reverseBack(A, n); return; }
  if ( tweak == 4 ) { tweakSort(A, n); return; }
  dither(A, n);
} // end slopes

void compareBentleyAgainstFivesort() {
  printf("Entering compareBentleyAgainstCut3 Sawtooth ........\n");
  // printf("Entering compareBentleyAgainstCut3 Rand2 ........\n");
  // printf("Entering compareBentleyAgainstCut3 Plateau ........\n");
  // printf("Entering compareBentleyAgainstCut3 Shuffle ........\n");
  // printf("Entering compareBentleygAainstCut3 Stagger ........\n");
  int bentleyTime, cut3Time, T;
  int seed = 666;
  int z;
  // int siz = 1024;
  int siz = 1024*1024;
  // int limit = 1024 * 1024 * 16 + 1;
  // int seedLimit = 32 * 1024;
  int limit = siz + 1;
  // int seedLimit = 32;
  int seedLimit = 1;
  float frac;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    // int A[siz];
    // void **A = malloc (sizeof(int) * siz);
    // create array
    struct intval *pi;
    void **A = myMalloc("compareBentleyAgainstFivesort 1", sizeof(pi) * siz);
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareBentleyAgainstFivesort 2", sizeof (struct intval));
      A[i] = pi;
    };

    // warm up the process
    fillarray(A, siz, seed);
    int TFill, m, tweak;
    int bentleyCnt, cut3Cnt; // , bentleyCntx, cut3Cntx;
    int sumQsortB, sumCut3; // , sumQsortBx, sumCut3x;
    // for (z = 0; z < 3; z++) { // repeat to check stability
    for (z = 0; z < 1; z++) { // repeat to check stability
      bentleyCnt = cut3Cnt = sumQsortB = sumCut3 = 0;
      // bentleyCntx = cut3Cntx = sumQsortBx = sumCut3x = 0;
      for (m = 1; m < 2 * siz; m = m * 2) {
      // m = 1024 * 1024; {
      	for (tweak = 0; tweak <= 5; tweak++ ) {
	  if (4 == tweak) continue; // due to bias 
	  bentleyTime = 0; cut3Time = 0;
	  TFill = clock();
	  for (seed = 0; seed < seedLimit; seed++) 
	    // sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak); // not used
	    // shuffle(A, siz, m, tweak, seed); // not used
	    // stagger(A, siz, m, tweak);
	    slopes(A, siz, m, tweak);
	  TFill = clock() - TFill;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    // sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak); // not used
	    // shuffle(A, siz, m, tweak, seed);  // not used
	    // stagger(A, siz, m, tweak);
	    slopes(A, siz, m, tweak);
	    // callBentley(A, 0, siz-1); 
	    callCut2(A, siz, compareIntVal); 
	    // callQuicksort0(B, siz, compareIntVal); 
	  }
	  bentleyTime = bentleyTime + clock() - T - TFill;
	  sumQsortB += bentleyTime;
	  // if ( 4 != tweak ) sumQsortBx += bentleyTime;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    // sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak); // not used
	    // shuffle(A, siz, m, tweak, seed); // not used
	    // stagger(A, siz, m, tweak);
	    slopes(A, siz, m, tweak);
	    fivesort(A, siz, compareIntVal);
	  }
	  cut3Time = cut3Time + clock() - T - TFill;
	  sumCut3 += cut3Time;
	  // if ( 4 != tweak ) sumCut3x += cut3Time;
	  printf("Plateau size: %d m: %d tweak: %d ", siz, m, tweak);
	  printf("bentleyTime: %d ", bentleyTime);
	  printf("Cut3Time: %d ", cut3Time);
	  frac = 0;
	  if ( bentleyTime != 0 ) frac = cut3Time / ( 1.0 * bentleyTime );
	  printf("frac: %f \n", frac);
	  if ( bentleyTime < cut3Time ) bentleyCnt++;
	  else cut3Cnt++;
	}
	printf("sumQsortB:   %i sumCut3:  %i frac: %f", 
	       sumQsortB, sumCut3, (sumCut3/(1.0 * sumQsortB)));
	printf(" bentleyCnt:  %i cut3Cnt:  %i\n", bentleyCnt, cut3Cnt);
      }
      frac = 0;
      if ( sumQsortB != 0 ) frac = sumCut3 / ( 1.0 * sumQsortB );
      printf("Measurements:\n");
      printf("sumQsortB:   %i sumCut3:  %i frac: %f", 
	     sumQsortB, sumCut3, (sumCut3/(1.0 * sumQsortB)));
      printf(" bentleyCnt:  %i cut3Cnt:  %i\n", bentleyCnt, cut3Cnt);
      // printf("sumQsortBx:  %i sumCut3x: %i", sumQsortBx, sumCut3x);
      // printf(" bentleyCntx: %i cut3Cntx: %i\n", bentleyCntx, cut3Cntx);
    }
    free(A);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
  }
} // end compareBentleyAgainstFivesort

void heapSort(void **a, int count, int (*compar)());
void callHeapsort(void **A, int size, 
	 int (*compar) (const void *, const void * ) ) {
  heapSort(A, size, compar);
} // end callHeapsort

void validateFiveSortBT() {
  printf("Entering validateFiveSortBT Sawtooth ........\n");
  // printf("Entering validateFiveSortBT Rand2 ........\n");
  // printf("Entering validateFiveSortBT Plateau ........\n");
  // printf("Entering validateFiveSortBT Shuffle ........\n");
  // printf("Entering validateFiveSortBT Stagger ........\n");
  int callBentleyTime, cut2Time, T;
  int seed =  666;
  int z;
  int siz = 1024*1024;
  // int limit = 1024 * 1024 * 16 + 1;
  // int seedLimit = 32 * 1024;
  int limit = siz + 1;
  // int seedLimit = 32;
  int seedLimit = 1;
  float frac;
  while (siz <= limit) {
    printf("%s %d %s %d %s", "siz: ", siz, " seedLimit: ", seedLimit, "\n");
    // int A[siz];
    struct intval *pi;
    void **A = myMalloc("compareAlgorithms0 1", sizeof(pi) * siz);
    void **B = myMalloc("compareAlgorithms0 3", sizeof(pi) * siz);
    // construct array
    int i;
    for (i = 0; i < siz; i++) {
      pi = myMalloc("compareAlgorithms0 2", sizeof (struct intval));
      A[i] = pi;
      pi = myMalloc("compareAlgorithms0 4", sizeof (struct intval));
      B[i] = pi;
    };
    // warm up the process
    fillarray(A, siz, seed);
    int TFill, m, tweak;
    int callBentleyCnt, cut2Cnt; // , callBentleyCntx, cut2Cntx;
    int sumQsortB, sumCut2; // , sumQsortBx, sumCut2x;
    // for (z = 0; z < 3; z++) { // repeat to check stability
    for (z = 0; z < 1; z++) { // repeat to check stability
      callBentleyCnt = cut2Cnt = sumQsortB = sumCut2 = 0;
      // callBentleyCntx = cut2Cntx = sumQsortBx = sumCut2x = 0;
      for (m = 1; m < 2 * siz; m = m * 2) {
      // m = 1024 * 1024; {
      // m = 1; {
      	for (tweak = 0; tweak <= 5; tweak++ ) {
	// tweak = 5; {
	  callBentleyTime = 0; cut2Time = 0;
	  TFill = clock();
	  for (seed = 0; seed < seedLimit; seed++) 
	    sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak);
	    // shuffle(A, siz, m, tweak, seed);
	    // stagger(A, siz, m, tweak);
	  TFill = clock() - TFill;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    sawtooth(A, siz, m, tweak);
	    // rand2(A, siz, m, tweak, seed);
	    // plateau(A, siz, m, tweak);
	    // shuffle(A, siz, m, tweak, seed);
	    // stagger(A, siz, m, tweak);
	    callHeapsort(A, siz, compareIntVal);
	  }
	  callBentleyTime = callBentleyTime + clock() - T - TFill;
	  sumQsortB += callBentleyTime;
	  // if ( 4 != tweak ) sumQsortBx += callBentleyTime;
	  T = clock();
	  for (seed = 0; seed < seedLimit; seed++) { 
	    sawtooth(B, siz, m, tweak);
	    // rand2(B, siz, m, tweak, seed);
	    // plateau(B, siz, m, tweak);
	    // shuffle(B, siz, m, tweak, seed);
	    // stagger(B, siz, m, tweak);
	    fivesort(B, siz, compareIntVal);  
	  }
	  cut2Time = cut2Time + clock() - T - TFill;
	  sumCut2 += cut2Time;
	  // if ( 4 != tweak ) sumCut2x += cut2Time;
	  printf("Size: %d m: %d tweak: %d ", siz, m, tweak);
	  printf("callBentleyTime: %d ", callBentleyTime);
	  printf("Cut2Time: %d ", cut2Time);
	  frac = 0;
	  if ( callBentleyTime != 0 ) frac = cut2Time / ( 1.0 * callBentleyTime );
	  printf("frac: %f \n", frac);
	  if ( callBentleyTime < cut2Time ) callBentleyCnt++;
	  else cut2Cnt++;
	  for (i = 0; i < siz; i++) {
	    if ( compareIntVal(A[i], B[i]) != 0 ) {
	      printf("***** validateFiveSortBT m: %i tweak: %i at i: %i\n", 
		     m, tweak, i);
	      exit(0);
	    }
	  }
	}
	printf("sumQsortB:   %i sumCut2:  %i frac: %f", 
	       sumQsortB, sumCut2, (sumCut2/(1.0 * sumQsortB)));
	printf(" callBentleyCnt:  %i cut2Cnt:  %i\n", callBentleyCnt, cut2Cnt);
      }
      frac = 0;
      if ( sumQsortB != 0 ) frac = sumCut2 / ( 1.0 * sumQsortB );
      printf("Measurements:\n");
      printf("sumQsortB:   %i sumCut2:  %i frac: %f", 
	     sumQsortB, sumCut2, (sumCut2/(1.0 * sumQsortB)));
      printf(" callBentleyCnt:  %i cut2Cnt:  %i\n", callBentleyCnt, cut2Cnt);
      // printf("sumQsortBx:  %i sumCut2x: %i", sumQsortBx, sumCut2x);
      // printf(" callBentleyCntx: %i cut2Cntx: %i\n", callBentleyCntx, cut2Cntx);

    }
    // free array
    for (i = 0; i < siz; i++) {
      free(A[i]); free(B[i]);
    };
    free(A); free(B);
    siz = siz * 2;
    seedLimit = seedLimit / 2;
  }
} // end validateFiveSortBT
