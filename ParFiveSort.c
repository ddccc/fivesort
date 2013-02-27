/*
Copyright (c) 2012, Dennis de Champeaux.  All rights reserved.

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

// File: c:/bsd/rigel/sort/ParFiveSort.c
// Date: Sun Jul 08 20:47:35 2012
/* This file has the source of the algorithms that make up the 
   parallel version of FiveSort headed by fivesort
*/
/* compile with: 
   gcc -c FiveSort.c
   which produces:
   FiveSort.o
   Compile & link with UseFiveSort:
   gcc UseFiveSort.c FiveSort.o
*/
// To adjust restrictions: go to fivesort(...) at the end of this file 


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

const int cut2Limit = 250;
const int cut3PLimit = 250;

#define errexit(code,str)                          \
  fprintf(stderr,"%s: %s\n",(str),strerror(code)); \
  exit(1);

// Here more global entities used throughout
int (*compareXY)();
void **A;
int sleepingThreads = 0;
int NUMTHREADS;

char* expiration = "*** License for fivesort has expired ...\n";

void iswap(int p, int q, void **A) {
  void *t = A[p];
  A[p] = A[q];
  A[q] = t;
} // end of iswap

// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med

void insertionsort(int N, int M) {
  if ( M <= N ) return;
  // M <= N + 10 
  register int i, s;
  int minimumPosition;
  void *minimum, *next, *ai;
  minimumPosition = N;
  minimum = A[N];
  for ( i = N+1; i <= M; i++ ) {
    ai = A[i];
    // if ( ai < minimum ) {
    if ( compareXY(ai, minimum) < 0 ) {
      minimum = ai;
      minimumPosition = i;
    }
  }
  if ( N != minimumPosition ) {
    A[minimumPosition] = A[N];
    A[N] = minimum;
  }
  s = N+1;
  // if ( M == s ) return;
  while ( s < M ) {
    s=s+1;
    next=A[s];
    ai = A[s-1];
    // if ( ai <= next ) continue;
    if ( compareXY(ai, next) <= 0 ) continue;
    A[s] = ai;
    ai = A[s-2];
    if ( compareXY(ai, next) <= 0 ) { A[s-1] = next; continue; }
    A[s-1] = ai;
    ai = A[s-3];
    if ( compareXY(ai, next) <= 0 ) { A[s-2] = next; continue; }
    A[s-2] = ai;
    ai = A[s-4];
    if ( compareXY(ai, next) <= 0 ) { A[s-3] = next; continue; }
    A[s-3] = ai;
    ai = A[s-5];
    if ( compareXY(ai, next) <= 0 ) { A[s-4] = next; continue; }
    A[s-4] = ai;
    ai = A[s-6];
    if ( compareXY(ai, next) <= 0 ) { A[s-5] = next; continue; }
    A[s-5] = ai;
    ai = A[s-7];
    if ( compareXY(ai, next) <= 0 ) { A[s-6] = next; continue; }
    A[s-6] = ai;
    ai = A[s-8];
    if ( compareXY(ai, next) <= 0 ) { A[s-7] = next; continue; }
    A[s-7] = ai;
    ai = A[s-9];
    if ( compareXY(ai, next) <= 0 ) { A[s-8] = next; continue; }
    A[s-8] = ai;
    ai = A[s-10];
    if ( compareXY(ai, next) <= 0 ) { A[s-9] = next; continue; }
    A[s-9] = ai;
    ai = A[s-11];
    if ( compareXY(ai, next) <= 0 ) { A[s-10] = next; continue; }
    A[s-10] = ai;
    ai = A[s-12];
    if ( compareXY(ai, next) <= 0 ) { A[s-11] = next; continue; }
    fprintf(stderr, "FiveSort/ insertionsort() ");
    fprintf(stderr, "N: %d M %d s: s\n", N, M, s);
    exit(1);
  }
  return;
}

void quicksort0c();
// Quicksort function for invoking quicksort0c.
void quicksort0(int N, int M) {
  int L = M - N;
  if ( L <= 0 ) return;
  // printf("quicksort0 %d %d \n", N, M);
  if ( L <= 10 ) { 
    insertionsort(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  quicksort0c(N, M, depthLimit);
} // end quicksort0

void heapc();
// Quicksort equipped with a defense against quadratic explosion;
// calling heapsort if depthlimit exhausted
void quicksort0c(int N, int M, int depthLimit) {
  // printf("Enter quicksort0c N: %d M: %d %d\n", N, M, depthLimit);
  while ( N < M ) {
    int L = M - N;
    if ( L <= 10 ) {
      insertionsort(N, M);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, N, M);
      return;
    }
    depthLimit--;
    // 10 < L
    // grab median of 3 or 9 to get a good pivot
    int pn = N;
    int pm = M;
    int p0 = (pn+pm)/2;
    if ( 40 < L ) { // do median of 9
      int d = L/8;
      pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
      p0 = med(A, p0 - d, p0, p0 + d, compareXY);
      pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      /* Activation of the check for duplicates gives a slow down of 
	 1/4% on uniform input.  If you suspect that duplicates
	 causing quadratic deterioration are not caught higher-up by cut3P
	 you may want to experiment with this check::::
      if ( L < 100 ) { // check for duplicates
	int duplicate = -1;
	if ( compareXY(A[pn], A[pm]) == 0 ) { duplicate = pn; } else
	if ( compareXY(A[pn], A[p0]) == 0 ) { duplicate = p0; } else
	if ( compareXY(A[pm], A[p0]) == 0 ) { duplicate = p0; };
	if ( 0 <= duplicate ) {
	  void quicksort0();
	  cut3duplicates(N, M, duplicate, quicksort0, depthLimit);
	  return;
	}
      }
      */
    }
    p0 = med(A, pn, p0, pm, compareXY); // p0 is index to 'best' pivot ...
    iswap(N, p0, A); // ... and is put in first position as required by quicksort0c

    register void *p = A[N]; // pivot
    register int i, j;
    i = N;
    j = M;
    register void *ai; void *aj;

    /* Split array A[N,M], N<M in two segments using pivot p; 
       construct a partition with A[N,i), A(i,M] and N <= i <= M, and
       N <= k <= i -> A[k] <= p  and  i < k <= M -> p < A[k];
       Allow the worse cases: N=i or i=M.
       Recurse on A[N,i) and A(i,M) (or in the reverse order).
       This code does NOT do swapping; instead it disposes 
       ai/aj in a hole created by setting aj/ai first.  
    */
    /* Start state:
	  |-------------------------------|
          N=i                           j=M
	  N = i < j = M
          N <= k < i -> A[k] <= p    
          N < j < k <= M -> p < A[k]
	  A[N] = p
          N < i -> p < A[i]
    */
    while ( i < j ) {
      /*
	  |-------o---------------[--------|
          N       i               j        M
	  N <= i < j <= M
          N <= k < i -> A[k] <= p    
          N < j < k <= M -> p < A[k]
	  A[N] <= p
          N < i -> p < A[i]
          p + A[N,i) + A(i,M] is a permutation of the input array
      */
      aj = A[j];
      while ( compareXY(p, aj) < 0 ) { 
        /*
	  |-------o---------------[--------|
          N       i               j        M
	  N = i < j <= M or N < i <= j <= M
          N <= k < i -> A[k] <= p    
          N < j <= k <= M -> p < A[k]
	  A[N] <= p
	  N < i -> p < A[i}
	  p + A[N,i) + A(i,M] is a permutation of the input array
          p < aj = A[j]
	*/
	j--; 
	aj = A[j]; 
      }
      /*
	  |-------o---------------[--------|
          N       i               j        M
	  N = i = j < M or N < i & = i-1 <= j <= M
          N <= k < i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  N < i -> p < A[i}
	  p + A[N,i) + A(i,M] is a permutation of the input array
          aj = A[j] <= p
	*/
      if ( j <= i ) {
	/*
	  |-------o-----------------------|
          N       i                       M
	  N = i = j < M or N < i & = i-1 = j < M
          N <= k < i -> A[k] <= p    
          i < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,i) + A(i,M] is a permutation of the input array
      */
	break;
      }
      // i < j 
      A[i] = aj; // fill hole !
      /*
	  |-------]---------------o--------|
          N       i               j        M
	  N <= i < j <= M
          N <= k <= i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,j) + A(j,M] is a permutation of the input array
	  aj = A[j] <= p
      */
      i++; ai = A[i];
      while ( i < j && compareXY(ai, p) <= 0 ) {
      /*
	  |-------]---------------o--------|
          N       i               j        M
	  N < i < j <= M
          N <= k <= i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,j) + A(j,M] is a permutation of the input array
	  aj = A[j] <= p
      */
	i++; ai = A[i]; 
      }
      if ( j <= i ) 
      /*
	  |----------------------o--------|
          N                     i=j       M
	  N < i = j <= M
          N <= k < i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
	  p + A[N,j) + A(j,M] is a permutation of the input array
      */
	break;
      // i < j  & p < ai = A[i] 
      A[j] = ai;
      j--;
      /*
	  |--------o--------------[--------|
          N        i              j        M
	  N < i <= j <= M
          N <= k < i -> A[k] <= p    
          j < k <= M -> p < A[k]
	  A[N] <= p
          N < i -> p < ai = A[i]
	  p + A[N,i) + A(i,M] is a permutation of the input array
      */
    }
    A[i] = p;
    /*
	  |--------]----------------------|
          N        i                      M
	  N <= i <= M
          N <= k <= i -> A[k] <= p    
          i < k <= M -> p < A[k]
	  A[N] <= p
	  A[N,i] + A(i,M] is a permutation of the input array
    */
    // Recurse on the smallest one and iterate on the other one
    int ia = i-1; int ib = i+1; 
    if ( i-N < M-i ) { 
      if ( N < ia ) quicksort0c(N, ia, depthLimit);  
      N = ib; 
    } else { 
      if ( ib < M ) quicksort0c(ib, M, depthLimit);  
      M = ia; 
    }
  }
} // end of quicksort0c

void cut2c();
// cut2 is a support function to call up the workhorse cut2c
void cut2(int N, int M) { 
  // printf("cut2 %d %d \n", N, M);
  int L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  cut2c(N, M, depthLimit);
} // end cut2

void cut3duplicates();
// Cut2 does 2-partitioning with one pivot.
// It can be used for comparisons.
// Cut2 invokes cut3duplicates when duplicates are found.
// cut2 is used as a best in class quicksort implementation 
// {with a defense against quadratic behavior due to duplicates}
void cut2c(int N, int M, int depthLimit) {
  int I, J; // indices
  void *AI, *AJ; // array values
  int L;
Start:
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0c(N, M, depthLimit);
    return;
  }
  depthLimit--;
 
  // Check for duplicates
        int sixth = (M - N + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
        int e3 = (N+M) / 2; // The midpoint
        int e4 = e3 + sixth;
        int e2 = e3 - sixth;

        // Sort these elements using a 5-element sorting network
        void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
	void *t;
        // if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
	int r; void *duplicate = NULL;
	r = compareXY(ae1, ae2);
	if ( 0 == r ) duplicate = ae2; else                  // 1-2
	  if ( 0 < r ) { t = ae1; ae1 = ae2; ae2 = t; }

	r = compareXY(ae4, ae5);
	if ( 0 == r ) duplicate = ae4; else                  // 4-5
	  if ( 0 < r ) { t = ae4; ae4 = ae5; ae5 = t; }

	r = compareXY(ae1, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 1-3
	  if ( 0 < r ) { t = ae1; ae1 = ae3; ae3 = t; }

	r = compareXY(ae2, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 2-3
	  if ( 0 < r ) { t = ae2; ae2 = ae3; ae3 = t; }

	r = compareXY(ae1, ae4);
	if ( 0 == r ) duplicate = ae4; else
	  if ( 0 < r ) { t = ae1; ae1 = ae4; ae4 = t; }      // 1-4

	r = compareXY(ae3, ae4);
	if ( 0 == r ) duplicate = ae3; else
	  if ( 0 < r ) { t = ae3; ae3 = ae4; ae4 = t; }      // 3-4

	r = compareXY(ae2, ae5);
	if ( 0 == r ) duplicate = ae2; else
	  if ( 0 < r ) { t = ae2; ae2 = ae5; ae5 = t; }      // 2-5

	r = compareXY(ae2, ae3);
	if ( 0 == r ) duplicate = ae3; else                  // 2-3
	  if ( 0 < r ) { t = ae2; ae2 = ae3; ae3 = t; }

	r = compareXY(ae4, ae5);                             // 4-5
	if ( 0 == r ) duplicate = ae4; else
	  if ( 0 < r ) { t = ae4; ae4 = ae5; ae5 = t; }
	// ... and reassign
	A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

	if ( NULL != duplicate ) { // found a duplicate thus delegate
	  int duplicatex = 0;
	  if ( duplicate == ae4 ) { duplicatex = e4; } else
	  if ( duplicate == ae3 ) { duplicatex = e3; } else
	  if ( duplicate == ae2 ) { duplicatex = e2; } else
	  if ( duplicate == ae1 ) { duplicatex = e1; } else
	  if ( duplicate == ae5 ) { duplicatex = e5; } else
	    { goto skip; } // defensive
	  void cut2c();
	  cut3duplicates(N, M, duplicatex, cut2c, depthLimit);
	  return;
	}
 skip:

	// Fix end points
	if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
	if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);

	register void *T = ae3; // pivot

	// initialize running indices
	I= N;
	J= M;

	// The left segment has elements < T
	// The right segment has elements >= T
  Left:
	 I = I + 1;
	 AI = A[I];
	 // if (AI < T) goto Left;
	 if ( compareXY( AI,  T) < 0 ) goto Left;
  Right:
	 J = J - 1;
	 AJ = A[J];
	 // if ( T <= AJ ) goto Right;
	 if ( compareXY( T, AJ) <= 0 ) goto Right;
	 if ( I < J ) {
	   A[I] = AJ; A[J] = AI;
	   goto Left;
	 }
  	 if ( (I - N) < (M - J) ) { // smallest one first
	   cut2c(N, J, depthLimit);
	   N = I; 
	   goto Start;
	 }
	 cut2c(I, M, depthLimit);
	 M = J;
	 goto Start;
} // (*  OF cut2c; *) ... the brackets reminds that this was Pascal code

/* deadx is NOT used for sorting.  It can be employed for 
   a test to invoke cut3duplicates */
void deadxc();
void deadx(int N, int M) {
  // printf("deadx N: %d M: %d\n", N, M);
  int L = M - N;
  if ( L <= 1 ) return;    
  int depthLimit = 2 * floor(log(L));
  deadxc(N, M, depthLimit);
} // end deadx

/* deadxc is also NOT used for sorting.  It is employed for 
   a test to invoke cut3duplicates */
void deadxc(int N, int M, int depthLimit) {
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  int L = M - N;  
  if ( L < cut3PLimit ) { 
    quicksort0c(N, M, depthLimit);
    return;
  }
  depthLimit--;

    int pivotx;
    int midpointx = (M+N)/2;
    if ( L <= 30 ) { // being defensive
      pivotx = med(A, N, midpointx, M, compareXY);
    } else {
      int step = L * 0.1;
      int P = med(A, N +   step, N + 2*step, N + 3*step, compareXY);
      int Q = med(A, midpointx - step, midpointx, midpointx + step, compareXY);
      int R = med(A, M - 3*step, M - 2*step, M - step, compareXY);
      pivotx = med(A, P, Q, R, compareXY);
    }

    void deadxc();
    cut3duplicates(N, M, pivotx, deadxc, depthLimit);
} // end deadxc

/* Three partition sorting with a single pivot p
   Left elements are less than the pivot
   Middle elements are equal to the pivot
   Right elements are greater than the pivot
   Only middle elements are guaranteed to be non-empty
   This module is invoked on 'pathological' inputs */
void cut3duplicates(int N, int M, int pivotx, void (*cut)(), int depthLimit) {
  // pivotx is the index inside [N,M] for pivot p.  *** DO NOT CHANGE ***
  register void *p = A[pivotx]; // pivot
  // indices 
  register int i, j, lw, up;
  // values
  register void *x, *y, *AM;
  register int r; // used for capturing output of comparison operation

  // initialize end points
  x = A[N]; AM = A[M];
  A[N] = A[M] = p;

  /* We employ dispose/whack-a-mole. We know in which partition element x 
     should be.  Find a close, undecided position where x should go.  
     Store its content in y.  Drop x (simplified).  Set x to y and repeat. 
   */
  // initialize running indices
  i = N;
  j = M;
  //  if ( x < p ) goto StartL;
  r = compareXY(x, p);
  if ( r < 0 ) goto StartL;
  //  if ( p < x ) goto StartR;
  if ( r > 0 ) goto StartR;
  goto CreateMiddle;

 StartL:
  /*
    |o---]-----------------------------[---o|
    N    i                             j    M
    x -> L
   */
    i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto StartL; 
  // if ( p < y ) { 
  if ( 0 < r ) { 
    if ( i < j ) {
      A[i] = x;
      x = y;
      goto StartR;
    }
    // gap closed; i==j 
    /*
    |o-----------][----------------------o|
    N             i                       M
    x -> L        j
   */
    A[N] = x;
    // AM must be p
    A[M] = A[j];
    A[j] = AM;
    goto Finish;
  }
  // y = p -> M
  if ( i < j ) {
    A[i] = x;
    x = y;
    goto CreateMiddle;
  }
  /* We have:
     x -> L
     y -> M
     i = j
     N+i <= k < i -> A[k] < p
     i = j <= k < M -> p < A[k]
     Therefore: 
     x must go to A[N] and 
     p = A[M], the only location where y can go.
     Hence: i = j = M 
     This cannot happen under normal use because there must be
     more than one element equal to p when used by the other 
     members of Par/SixSort

    |o------------------------------------]o|
    N                                      M=i=j
    x -> L
   */
  A[N] = x;
  // AM must be p hence the next assignment is defensive
  A[M] = AM;
  i--;
  goto FinishL;

 StartR:
  /*
    |o---]-----------------------------[---o|
    N    i                             j    M
    x -> R
   */
  j--;
  y = A[j];
  r = compareXY(y, p);
  if ( 0 < r ) goto StartR;
  if ( r < 0 ) {
    if ( i < j ) {
      A[j] = x;
      x = y;
      goto StartL;
    }
    // gap closed; i==j
    /*
    |o-----------][----------------------o|
    N            i                        M
    x -> R       j
   */
    A[M] = x;
    // AM must be p
    A[N] = A[i]; A[i] = AM;
    goto Finish;
  }
  // y -> M
  if ( i < j ) {
    A[j] = x;
    x = y;
    goto CreateMiddle;
  }
  /* We have:
     x -> R
     y -> M
     i = j
     N+i <= k <= i -> A[k] < p
     i = j < k < M -> p < A[k]
     Therefore: 
     x must go to A[M] and 
     p = A[N], the only location where y can go.
     Hence: i = j = N
     This cannot happen under normal use because there must be
     more than one element equal to p when used by the other 
     members of Par/SixSort.

    |o][-----------------------------------o|
    N=i=j                                   M
    x -> R
   */
  A[M] = x;
  // AM must be p
  A[N] = AM;
  j++;
  goto FinishR;

 CreateMiddle:
  /*
    |o---]-----------------------------[---o|
    N    i                             j    M
    x -> M
   */
  if ( i+1 == j ) {
    /*
    |o--------------][---------------------o|
    N               ij                      M
    x -> M
    */
    // if ( AM < p ) {
    r = compareXY(AM, p);
    if ( r < 0 ) {
      A[N] = AM;
      A[M] = A[j]; A[j] = x;
      i++;
      goto Finish;
    } 
    //    if ( p < AM ) {  
    if ( 0 < r ) {
      A[M] = AM;
      A[N] = A[i]; A[i] = x;
      j--;
      goto Finish;
    }
    // p = AM
    A[N] = A[i]; A[i] = x; 
    A[M] = A[j]; A[j] = AM; 
    goto Finish;
  }
  lw = (i+j)/ 2;
  up = lw+1;
  y = A[lw];
  A[lw] = x;
  x = y;
  lw--;

  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
   x -> ?
   */
  // if ( x < p ){
  r = compareXY(x, p);

  if ( r < 0 ) {
    goto LLgMgRL;
  }
  // if ( p < x ) {
  if ( 0 < r ) {
    goto LLgMgRR;
  }
  goto LLgMgRM1;

 LLgMgRL:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> L
   */
  i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto LLgMgRL;
  if ( 0 < r ) {
    A[i] = x;
    x = y;
    goto LLgMgRR;
  }
  // y -> M
  if ( i <= lw ) {
    A[i] = x;
    x = y;
    goto LLgMgRM1;
  }
  // left gap closed
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> L
   */
  goto LLMgRL;
  
 LLgMgRR:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> R
   */
  j--;
  y = A[j];
  r = compareXY(p, y );
  if ( r < 0 ) goto LLgMgRR;
  if ( 0 < r ) {
    A[j] = x;
    x = y;
    goto LLgMgRL;
  }
  // y -> M
  if ( up <= j ) {
    A[j] = x;
    x = y;
    goto LLgMgRM2;
  }
  // right gap closed
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> R
   */
  goto LLgMRR;
  
 LLgMgRM1:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> M
   */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    A[lw] = x;
    x = y;
    lw--; 
    goto LLgMgRR;
  } 
  if ( r == 0 ) {
   if ( i < lw ) {
      lw--;
      goto LLgMgRM2;
    }
    goto LMgRM;
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = x;
    x = y;
    lw--;
    goto LLgMgRL;
  }
  // left gap closed
  i++; 
  goto LLMgRM;
    
 LLgMgRM2:
  /*
    |o---]-----------+[-]+-------------[---o|
    N    i          lw   up            j    M
    x -> M
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) {
    A[up] = x;
    x = y;
    up++;
    goto LLgMgRL;
  }
  if ( r == 0 ) {
    if ( up < j ) {
      up++;
      goto LLgMgRM1;
    }
    goto LLgMM;
  }
  // y -> R
  if ( up < j ) {
    A[up] = x;
    x = y;
    up++;
    goto LLgMgRR;
  }
  // right gap closed
  j--; 
  goto LLgMRM;

  // left gap closed
 LLMgRL:  
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> L
  */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) {
    A[up] = A[i]; A[i] = y;
    up++; i++;
    goto LLMgRL;
  }
  // if ( y == p ) {
  if ( r == 0 ) { 
    if ( up == M ) {
   /*
    |o---------][-------------------]o|
    N           i                     M
    x -> L
   */
      A[N] = x;
      if ( compareXY(p, AM) <= 0 ) {
	A[M] = AM;
	i--;
	goto FinishL;
      }
      /*
      // if ( p == AM ) { 
      if ( compareXY(p, AM) == 0 ) { 
	A[M] = AM;
	i--;
	goto FinishL;
      }
      */
      // compareXY(AM, p) < 0
      A[M] = A[i]; A[i] = AM;
      goto FinishL;
    }
    up++;
    goto LLMgRL;
  }
  // y -> R
  if ( up < j ) {
    A[up] = A[i]; A[i] = x;
    x = y;
    i++; up++;
    goto LLMgRR;
  }
  // right gap closed also
  /*
    |o---------][------][-----------------o|
    N           i       j                  M
    x -> L
   */
  // up == j; A[j] == y 
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[N] = x; A[M] = AM;
    j--;
    goto Finish;
  }
  if ( r == 0 ) {
    A[N] = x; A[M] = y; A[j] = AM;
    goto Finish;
  }
  // AM -> L and x -> L
  A[M] = y; A[j] = A[i];
  // if ( x <= AM ) { 
  if ( compareXY(x, AM ) <= 0 ) { 
    A[N] = x; 
    A[i] = AM;
  } else {
    A[N] = AM; 
    A[i] = x;
  }
  i++;
  goto Finish;

 LLMgRM:
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> M
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) {
    A[up] = A[i]; A[i] = y; 
    i++; up++;
    goto LLMgRM;
  }
  if ( r == 0 ) {
    if ( up == M ) { // R empty 
      /*
    |o---------][-------------------]o|
    N           i                     M
    x -> M
      */
      if ( compareXY(AM, p) < 0 ) {
	A[N] = AM;
	A[M] = x;
	i--;
	goto FinishL;
      }
      /*
      if ( compareXY(p, AM) < 0 ) {
	A[M] = AM;
	i--;
	A[N] = A[i]; A[i] = x;
	i--;
	goto FinishL;
      }
      */
      A[M] = AM;
      i--;
      A[N] = A[i]; A[i] = x;
      i--;
      goto FinishL;
    }
    up++;
    goto LLMgRM;
  }
  // y -> R
  if ( up < j ) {
    A[up] = x;
    x = y;
    up++;
    goto LLMgRR;
  }
  // right gap closed also
  /*
    |o---------][------][-----------------o|
    N           i       j                  M
    x -> M
   */
  // up == j; A[j] == y 
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[M] = AM;
    i--;
    A[N] = A[i]; A[i] = x;
    j--;
    goto Finish;
  } 
  if ( 0 < r ) {
    A[N] = AM; A[M] = y; A[j] = x;
    goto Finish;
  }
  // AM -> M
  i--;
  A[N] = A[i]; A[i] = x;
  A[M] = y; A[j] = AM;
  goto Finish;

 LLMgRR:
  /*
    |o---------][------]+-------------[---o|
    N           i       up            j    M
    x -> R
  */
  j--;
  y = A[j];
  r = compareXY(p, y);
  if ( r < 0 ) goto LLMgRR;
  if ( 0 < r ) {
    A[j] = x;
    x = y;
    goto LLMgRL;
  }
  // y -> M
  if ( up <= j ) {
    A[j] = x;
    x = y;
    goto LLMgRM;
  }
  // right gap closed also   y = A[j]
  /*
    |o---------][------][-----------------o|
    N           i      j                   M
    x -> R
   */
  A[M] = x;
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM; 
    goto Finish;
  }
  if ( r == 0 ) {
    i--;
    A[N] = A[i]; A[i] = AM;
    goto Finish;
  }
  // AM -> R
  i--;
  A[N] = A[i]; A[i] = y; A[j] = AM;
  j--; 
  goto Finish;

  // right gap closed
 LLgMRR:
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> R
  */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    A[lw] = A[j]; A[j] = y;
    lw--; j--;
    goto LLgMRR;    
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L is empty 
      /*
    |o[-----------------------][---------o|
    N                         j           M
    x -> R
      */
      A[M] = x;
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM;
	j++;
	goto FinishR;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = AM;
	j++;
	goto FinishR;
      }
      */
      // compareXY(p, AM) < 0
      A[N] = A[j]; A[j] = AM;
      goto FinishR;
    }
    lw--;
    goto LLgMRR; 
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = A[j]; A[j] = x;
    x = y;
    lw--; j--;
    goto LLgMRL;
  }
  // left gap closed also
  /*
    |o---][---------------------][---------o|
    N    i                      j           M
    x -> R
  */
  // y == A[lw] == A[i]
  A[M] = x;
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM;
    i++;
    goto Finish;
  }
  if ( r == 0 ) {
    A[N] = y; A[i] = AM;
    goto Finish;
  }
  // AM -> R
  A[N] = y; A[i] = A[j]; A[j] = AM;
  j--; 
  goto Finish;

 LLgMRM:
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> M
  */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    A[lw] = A[j]; A[j] = y;
    lw--; j--;
    goto LLgMRM;    
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L empty 
      /*
    |o[-----------------------][---------o|
    N                         j           M
    x -> M
      */
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM;
	j++;
	A[M] = A[j];
	A[j] = x;
	j++;
	goto FinishR; 
      }
      /*
      // if ( p == AM ) {
      if ( compareXY(p, AM) == 0 ) { 
	A[N] = AM;
	j++;
	A[M] = A[j];
	A[j] = x;
	j++;
	goto FinishR; 
      }
      */
      // compareXY(p, AM) < 0
      A[M] = AM; A[N] = x;
      j++;
      goto FinishR; 
    }
    lw--;
    goto LLgMRM; 
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = x;
    x = y;
    lw--;
    goto LLgMRL;
  }
  // left gap closed also
  /*
    |o---][---------------------][---------o|
    N    i                      j           M
    x -> M
  */
  // y == A[lw] == A[i]
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[M] = AM; 
    A[N] = y; A[i] = x;
    goto Finish;
  }
  if ( 0 < r ) {
    A[N] = AM; 
    j++;
    A[M] = A[j]; A[j] = x;
    i++;
    goto Finish;
  }
  // AM == p
  A[N] = y; A[i] = x;
  j++;
  A[M] = A[j]; A[j] = AM;
  goto Finish;

 LLgMRL:
  /*
    |o---]-----------+[---------][---------o|
    N    i          lw          j           M
    x -> L
   */
  i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto LLgMRL;
  if ( 0 < r ) {
    A[i] = x;
    x = y;
    goto LLgMRR;
  }
  // y -> M
  if ( i <= lw ) {
    A[i] = x;
    x = y;
    goto LLgMRM;  
  }
  // left gap closed also  y = A[i];
   /*
    |o---][----------------------][---------o|
    N     i                      j           M
    x -> L
   */
  A[N] = x;
  r = compareXY(p, AM);
  if ( r < 0 ) { 
    A[M] = AM;
    goto Finish;
  }
  if ( r == 0 ) {
    j++;
    A[M] = A[j]; A[j] = AM;
    goto Finish;
  }
  // AM -> L
  j++;
  A[M] = A[j]; A[j] = y; A[i] = AM;
  i++;
  goto Finish;

 LMgRM:
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> M
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) { // N == i 
    i++;
    A[up] = A[i]; A[i] = y;
    i++; up++;
    goto LLMgRM;
  }
  if ( r == 0 ) {
    if ( up == M ) {
   /*
    |o[----------------------------]o|
    N                                M
    x -> M
   */
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      */
      // compareXY(p, AM) < 0
      A[N] = x; A[M] = AM;
      goto Finish0;
    }
    up++;
    goto LMgRM;
  }
  // compareXY(p, y ) < 0
  if ( up < j ) {
    A[up] = x;
    x = y;
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> R
   */
    goto LMgRR;
  }
  // right gap closed  y = A[up] = A[j] -> R
  /*
    |o[-----------------------------][---o|
    N                                j    M
    x -> M
   */
  if ( compareXY(AM, p) <= 0 ) {
    A[N] = AM;
    A[M] = y; A[j] = x;
    j++;
    goto FinishR;
  }
  /*
  if ( compareXY(AM, p) < 0 ) {
    A[N] = AM;
    A[M] = A[j]; A[j] = x;
    j++;
    goto FinishR;
  }
  if ( compareXY(p, AM) == 0 ) {
    A[N] = AM;
    A[M] = A[j]; A[j] = x;
    j++;
    goto FinishR;
  }
  */
  // AM -> R
  A[N] = x; A[M] = AM;
  goto FinishR;

 LLgMM:
   /*
    |o---]-----------+[------------------]o|
    N    i          lw                     M
    x -> M
   */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    j--;
    A[lw] = A[j]; A[j] = y;
    lw--;
    j--;
    goto LLgMRM;
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L empty also 
      /*
    |o[----------------------------]o|
    N                                M
    x -> M
      */
      if ( compareXY(AM, p) <= 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = AM; A[M] = x;
	goto Finish0;
      }
      */
      // AM -> R
      A[N] = x; A[M] = AM;
      goto Finish0;
    }
    lw--;
    goto LLgMM;
  }
  // y -> L
  if ( i < lw ) {
    A[lw] = x;
    x = y;
    lw--;
    goto LLgML;
  }
  // left gap closed also  y = A[lw] = A[i] -> L
   /*
    |o---][-----------------------------]o|
    N    i                                M
    x -> M
   */
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM; A[M] = x;
    goto FinishL;
  }
  if ( r == 0 ) {
    A[N] = y; A[i] = AM; A[M] = x;
    i--;
    goto FinishL;
  }
  // compareXY(p, AM) < 0
  A[M] = AM;
  A[N] = y; A[i] = x;
  i--;
  goto FinishL;

 LLgML:
   /*
    |o---]-----------+[------------------]o|
    N    i          lw                     M=j
    x -> L
   */
  i++;
  y = A[i];
  r = compareXY(y, p);
  if ( r < 0 ) goto LLgML;
  if ( r == 0 ) {
    if ( i <= lw ) {
      A[i] = x;
      x = y;
      goto  LLgMM;
    }
    // left gap closed  y = A[i]
   /*
    |o--------------][------------------]o|
    N                i                    M=j
    x -> L
   */
    A[N] = x;
    if ( compareXY(p, AM) <= 0 ) {
      A[M] = AM;
      i--;
      goto FinishL;
    }
    /*
    // if ( p == AM ) {
    if ( compareXY(p, AM) == 0 ) { 
      A[M] = AM;
      i--;
      goto FinishL;
    }
    */
    // AM -> L
    A[M] = y; A[i] = AM;
    goto FinishL;
  }
  // y -> R
  A[i] = x;
  x = y;
  goto LLgMR;

 LLgMR:
   /*
    |o---]-----------+[------------------]o|
    N    i          lw                     M=j
    x -> R
   */
  y = A[lw];
  r = compareXY(p, y);
  if ( r < 0 ) {
    j--;
    A[lw] = A[j]; A[j] = y;
    j--; lw--;
    goto LLgMRR;
  }
  if ( r == 0 ) {
    if ( N == lw ) { // L is empty 
      /*
    |o[----------------------------]o|
    N                                M=j
    x -> R
      */
     if ( compareXY(AM, p) <= 0 ) { 
	A[N] = AM;
	A[M] = x;
	goto Finish0;
     }
     /*
     if ( compareXY(p, AM) == 0 ) { 
	A[N] = AM;
	A[M] = x;
	goto Finish0;
     }
     */
     // AM -> R
     j--;// = M-1
     A[N] = A[j];
     if ( x <= AM ) {
       A[j] = x; A[M] = AM;
     } else {
       A[j] = AM; A[M] = x;
     }
     goto Finish0;
    }
    lw--;
    goto LLgMR;
  }
  // y -> L
  if ( i < lw ) {
    j--;
    A[lw] = A[j]; A[j] = x;
    x = y;
    lw--; j--;
    goto LLgMRL;
  }
  // left gap closed y = A[lw] = A[i] -> L
 /*
    |o---][----------------------------]o|
    N    i                               M=j
    x -> R
   */
  r = compareXY(AM, p);
  if ( r < 0 ) {
    A[N] = AM; A[M] = x;
    goto FinishL;
  }
  if ( r == 0 ) {
    A[M] = x;
    A[N] = y; A[i] = AM;
    i--;
    goto FinishL;
  }
  // AM -> R
  j--;
  A[N] = y; A[i] = A[j];
  if ( x <= AM ) {
    A[j] = x; A[M] = AM;
  } else {
    A[j] = AM; A[M] = x;
  }
  i--;
  goto FinishL;

 LMgRR:
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> R
   */
  j--;
  y = A[j];
  r = compareXY(p, y);
  if ( r < 0 ) goto LMgRR;
  if ( 0 < r ) {
    A[j] = x;
    x = y;
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> L
   */
    goto LMgRL;
  }
  // y -> M
  if ( up <= j ) {
    A[j] = x;
    x = y;
    goto LMgRM;
  }
  // right gap closed  y = A[j]
  /*
    |o[---------------][----------------o|
    N                 j                  M
    x -> R
   */
  A[M] = x;
  if ( compareXY(AM, p) <= 0 ) {
    A[N] = AM;
    j++;
    goto FinishR;
  }
  /*
  if ( compareXY(p, AM) == 0 ) {
    A[N] = AM;
    j++;
    goto FinishR;
  }
  */
  // AM -> R
  A[N] = A[j]; A[j] = AM;
  goto FinishR;

 LMgRL:
   /*
    |o[---------------]+-------------[---o|
    N                  up            j    M
    x -> L
   */
  y = A[up];
  r = compareXY(y, p);
  if ( r < 0 ) { // create L
    i++;
    A[up] = A[i]; A[i] = y;
    i++; up++;
    goto LLMgRL;    
  }
  if ( r == 0 ) {
    if ( up == M ) { // R is empty 
      /*
    |o[----------------------------]o|
    N                                M
    x -> L
      */
      if ( compareXY(p, AM) <= 0 ) {
	A[N] = x; A[M] = AM;
	goto Finish0;
      }
      /*
      if ( compareXY(p, AM) == 0 ) {
	A[N] = x; A[M] = AM;
	goto Finish0;
      }
      */
      // AM -> L
      i++;
      A[M] = A[i];
      if ( x <= AM ) {
       A[N] = x; A[i] = AM;
      } else {
	A[N] = AM; A[i] = x;
      }
      goto Finish0;
    }
    up++;
    goto LMgRL;
  }
  // y -> R
  if ( up < j ) {
    i++;
    A[up] = A[i]; A[i] = x;
    x = y;
    i++; up++;
    goto LLMgRR;
  }
  // right gap closed   y = A[up] = A[j]
  /*
    |o[-----------------------------][---o|
    N                                j    M
    x -> L
   */
  r = compareXY(p, AM);
  if ( r < 0 ) {
    A[N] = x; A[M] = AM;
    goto FinishR;
  }
  if ( r == 0 ) {
    A[N] = x; 
    A[M] = A[j]; A[j] = AM;
    j++;
    goto FinishR;
  }
  // AM -> L
  i++;
  A[M] = A[j]; A[j] = A[i];
  if ( x <= AM ) {
    A[N] = x; A[i] = AM;
  } else {
    A[N] = AM; A[i] = x;
  }
  j--;
  goto FinishR;

  int k;

 Finish0:
  return;

 FinishL:
    (*cut)(N, i, depthLimit);
  return;

 FinishR:
    (*cut)(j, M, depthLimit);
  return;

 Finish:
  /* holes filled
    |o---------][------][-----------------o|
    N           i      j                   M
  */
  /*
  int k;
  if ( i <= N ) {
    printf("Error i-index\n");
    printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
    exit(1);
  }
  if ( j < i ) {
    printf("Error i-j-index\n");
    printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
    exit(1);
  }
  if ( M <= j ) {
    printf("Error j-index\n");
    printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
    exit(1);
  }
  for (k = N; k < i; k++) 
    // if ( p <= A[k] ) {
    if ( compareXY(p, A[k]) <= 0 ) {
      printf("Error L at k: %d \n", k);
      printf("N: %i i: %i j: %i M: %i\n", N, i, j, M);
      exit(1);
    }
  for (k = i; k <= j; k++) 
    // if ( p != A[k] ) {
    if ( compareXY(p, A[k]) != 0 ){
      printf("Error M at k: %d \n", k);
      printf("N: %d i: %d j: %d M: %d\n", N, i, j, M);
      exit(1);
    }
  for (k = j+1; k <= M; k++) 
    // if ( A[k] <= p ) {
    if ( compareXY(A[k], p) <= 0 ) {
      printf("Error R at k: %d \n", k);
      printf("N: %d i: %d j: %d M: %d\n", N, i, j, M);
      exit(1);
    }
  */
  if ( i - N  < M - j ) {
    (*cut)(N, i-1, depthLimit);
    (*cut)(j+1, M, depthLimit);
  } else {
    (*cut)(j+1, M, depthLimit);
    (*cut)(N, i-1);
  }
} // end of cut3duplicates



struct stack *ll;
struct task *newTask();
void addTaskSynchronized();


void cut3Pc();
// Three partition sorting with 2 pivots
void cut3P(int N, int M) {
  int L = M - N;
  if ( L < cut3PLimit ) { 
    quicksort0(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  cut3Pc(N, M, depthLimit);
} // end cut3P


// Three partition sorting with 2 pivots
void cut3Pc(int N, int M, int depthLimit) {
  int L;
 Begin:
  if ( depthLimit <= 0 ) {
    heapc(A, N, M);
    return;
  }
  // printf("cut3P %d %d \n", N, M);
  L = M - N;
  if ( L < cut3PLimit ) {
    quicksort0c(N, M, depthLimit);
    return; 
  }
  

  /*   L             M             R
    |-----]------+[------]+-----[-----|
    N     i      lw      up     j     M
   */

  register void *maxl, *minr;   // pivots for left/ right boundaries
  /* There are invariants to be maintained (and >essential< 
     for machine assisted correctness proofs):
     If there are two gaps:
       N <= x <= i --> A[x] <= maxl
       lw < x < up  --> maxl < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the left gap has closed:
       N <= x < i --> A[x] <= maxl
       i <= x < up  --> maxl < A[x] < minr
       j <= x <= M --> minr <= A[x]
     If the right gap has closed:
       N <= x <= i --> A[x] <= maxl
       lw < x <= j  --> maxl < A[x] < minr
       j < x <= M --> minr <= A[x]
  */

  int probeParamCut3Px = 50000;
  int probeLng = L/ probeParamCut3Px;
  if ( probeLng < 28 ) probeLng = 28; // quite short indeed
  int halfSegmentLng = probeLng/2;
  int middle = N + L/2;
  int N1 = middle - halfSegmentLng;
  int M1 = middle + halfSegmentLng;
  int oneThird = probeLng/3;
  int maxlx = N1 + oneThird;
  int minrx = M1 - oneThird;
  int offset = L/probeLng;
  // assemble the mini array [N1, M1]  
  int k;
  for (k = 0; k < probeLng; k++) iswap(N1 + k, N + k * offset, A);
  // sort this mini array to obtain good pivots
  quicksort0c(N1, M1, depthLimit);

  void *first = A[N1];
  maxl = A[maxlx];
  minr = A[minrx];
  void *last = A[M1];

  // test here for duplicates and go to cut3duplicates when dups found
  int dupx = -1; // candidate duplicate
  // if ( first == last ) dupx = N; else
 if ( compareXY(first, last) == 0 ) dupx = N1; else
  if ( compareXY(first, maxl) == 0 ) dupx = maxlx; else
  if ( compareXY(last, minr ) == 0) dupx = minrx;
  void cut3Pc();
  if ( 0 <= dupx ) {
    cut3duplicates(N, M, dupx, cut3Pc, depthLimit);
    return;
  }

  depthLimit--;


  // fix the end points
  if ( compareXY(A[N1], A[N]) < 0 ) iswap(N1, N, A);
  if ( compareXY(A[M], A[M1]) < 0 ) iswap(M1, M, A);

  register int i, j, lw, up; // indices
  register void *x, *y; // values

  // Ready to roll ...
  i = N;
  j = M;

Start:
         /*   L                           R
	   |-----]---------------------[-----|
	   N     i                     j     M
	 */
        i++;
	x = A[i];
	// if ( x <= maxl ) 
	if ( compareXY(x, maxl) <= 0 ) goto Start;
	A[i] = A[N];
	// if ( minr <= x ) 
	if ( compareXY(minr, x) <= 0 ) goto Right;
	goto CreateMiddle;

 Right:
         /*   L                           R
	   |o----]---------------------[-----|
	   N     i                     j     M
	   x -> R
	 */
	j--;
	y = A[j];
	// if ( minr <= y ) goto right;
	if ( compareXY(minr, y) <= 0 ) goto Right;
	// if ( maxl < y ) { 
	if ( compareXY(maxl, y) < 0 ) {
	  A[j] = x;
	  x = y;
	  goto CreateMiddle;
	}
	// y -> L 
	if ( i < j ) {
	  A[j] = x;
	  x = y;
	  goto Left;
	}
	// gap closed
	A[N] = A[i];
	A[i] = x;
	if ( i - N < M - j) {
	  // cut3Pc(N, i-1, depthLimit);
	  // N = i;
	  // goto Begin;
	  addTaskSynchronized(ll, newTask(i, M, depthLimit));
	  // cut3Pc(N, i-1, depthLimit);
	  // return;
	  M = i-1;
	  goto Begin;
	}
	// cut3Pc(i, M, depthLimit);
	// M = i - 1;
	// goto Begin;
	addTaskSynchronized(ll, newTask(N, i-1, depthLimit));
	// cut3Pc(i, M, depthLimit);
	// return;
	N = i;
	goto Begin;

 Left:
         /*   L                           R
	   |o----]---------------------[-----|
	   N     i                     j     M
	   x -> L
	 */
	i++;
	y = A[i];
	// if ( y <= maxl ) goto Left;
	if ( compareXY(y, maxl) <= 0 ) goto Left;
	// if ( y < minr ) { 
	if ( compareXY(y, minr) < 0 ) {
	  A[i] = x;
	  x = y;
	  goto CreateMiddle;
	}
	// y -> R but check whether gap closed
	if ( i < j ) {
	  A[i] = x;
	  x = y;
	  goto Right;
	}
	// gap closed
	A[N] = x;
	if ( i - N < M - j) {
	  // cut3Pc(N, i-1, depthLimit);
	  // N = i;
	  // goto Begin;
	  addTaskSynchronized(ll, newTask(i, M, depthLimit));
	  // cut3Pc(N, i-1, depthLimit);
	  // return;
	  M = i - 1;
	  goto Begin;
	}
	// cut3Pc(i, M, depthLimit);
	// M = i - 1;
	// goto Begin;
	addTaskSynchronized(ll, newTask(N, i-1, depthLimit));
	// cut3Pc(i, M, depthLimit);
	// return;
	N = i;
	goto Begin;

 CreateMiddle:
         /*   L                           R
	   |o----]---------------------[-----|
	   N     i                     j     M
	   x -> M
	 */
	lw = (i+j)/2;
	up = lw+1;
	y = A[lw];
	A[lw] = x;
	lw--;
	x = y;
	if ( compareXY(x, maxl) <= 0 )  
	  goto TLgMgRL; 
	if ( compareXY(minr, x) <= 0 )  
	  goto TLgMgRR; 
	goto TLgMgRM1;

 TLgMgRL:
        /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> L             
	 */
	/*
	for (k = N+1; k <= i; k++) 
	  if ( compareXY(maxl, A[k]) < 0 ) printf("TLgMgRL k: %i\n", k);
	for (k = lw+1; k < up; k++) 
	  if ( compareXY(A[k], maxl) <= 0 ||
	       compareXY(minr, A[k]) <= 0 ) printf("TLgMgRL k: %i\n", k);
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) printf("TLgMgRL k: %i\n", k);
	*/
	i++;
	y = A[i];
	// if ( y <= maxl ) 
	if ( compareXY(y, maxl) <= 0 ) 
	  goto TLgMgRL;
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) {
	  A[i] = x;
	  x = y;
	  goto TLgMgRR;
	}
	// y -> M
	if ( i <= lw ) {
	  A[i] = x;
	  x = y;
	  goto TLgMgRM1;
	}
	// left gap closed
	goto TLMgRL;

 TLgMgRR:
        /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> R             
	 */

	/*
	printf("TLgMgRR N: %i i: %i lw: %i up: %i j: %i M: %i \n", 
	       N, i, lw, up, j, M);
	for (k = N+1; k <= i; k++) 
	  if ( compareXY(maxl, A[k]) < 0 ) printf("TLgMgRR1 k: %i\n", k);
	for (k = lw+1; k < up; k++) 
	  if ( compareXY(A[k], maxl) <= 0 ||
	       compareXY(minr, A[k]) <= 0 ) printf("TLgMgRR2 k: %i\n", k);
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) printf("TLgMgRR3 k: %i\n", k);
	*/

	j--;
	y = A[j];
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) 
	  goto TLgMgRR;
	// if ( y <= maxl ) 
	if ( compareXY(y, maxl) <= 0 ) {
	  A[j] = x;
	  x = y;
	  /*
	for (k = N+1; k <= i; k++) 
	  if ( compareXY(maxl, A[k]) < 0 ) printf("TLgMgRR1x k: %i\n", k);
	for (k = lw+1; k < up; k++) 
	  if ( compareXY(A[k], maxl) <= 0 ||
	       compareXY(minr, A[k]) <= 0 ) printf("TLgMgRR2x k: %i\n", k);
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) printf("TLgMgRR3x k: %i\n", k);
	  */
	  goto TLgMgRL;
	}
	// y -> M
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
	  goto TLgMgRM2;
	}
	// right gap closed
	goto TLgMRR;

 TLgMgRM1:
        /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> M             
	 */
	/*
	for (k = N+1; k <= i; k++) 
	  if ( compareXY(maxl, A[k]) < 0 ) printf("TLgMgRM11 k: %i\n", k);
	for (k = lw+1; k < up; k++) 
	  if ( compareXY(A[k], maxl) <= 0 ||
	       compareXY(minr, A[k]) <= 0 ) printf("TLgMgRM12 k: %i\n", k);
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) printf("TLgMgRM13 k: %i\n", k);
	*/
	y = A[lw];
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = x;
	  lw--;
	  x = y;
	  goto TLgMgRR;
	}
	// if ( maxl < y )
	if ( compareXY(maxl, y) < 0 ) {
	  lw--;
	  goto TLgMgRM2;
	}
	// y -> L
	if ( lw <= i ) { // left gap closed 
	  i++;
	  goto TLMgRM;
	}
	A[lw] = x;
	lw--;
	x = y;
	goto TLgMgRL;

 TLgMgRM2:
        /*   L                            R
	   |o----]---------+[-]+--------[-----|
	   N     i        lw   up       j     M
           x -> M             
	 */
	/*
	for (k = N+1; k <= i; k++) 
	  if ( compareXY(maxl, A[k]) < 0 ) printf("TLgMgRM21 k: %i\n", k);
	for (k = lw+1; k < up; k++) 
	  if ( compareXY(A[k], maxl) <= 0 ||
	       compareXY(minr, A[k]) <= 0 ) printf("TLgMgRM22 k: %i\n", k);
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) printf("TLgMgRM23 k: %i\n", k);
	*/
	y = A[up];
	// if ( y <= maxl )
	if ( compareXY(y, maxl) <= 0 ) {
	  A[up] = x;
	  up++;
	  x = y;
	  goto TLgMgRL;
	}
	// if ( y < minr )
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLgMgRM1;
	}
	// y -> R
	if ( j <= up ) { // right gap closed
	  j--;
	  goto TLgMRM;
	}
	A[up] = x;
	up++;
	x = y;
	goto TLgMgRR;

 TLgMRM:
        /*   L                            R
	   |o----]---------+[-----][---------|
	   N     i        lw      j          M
           x -> M             
	 */
	y = A[lw];
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[j];
	  lw--;
	  A[j] = y;
	  j--;
	  goto TLgMRM;
	}
	// if ( maxl < y )
	if ( compareXY(maxl, y) < 0 ) {
	  lw--;
	  goto TLgMRM;
	}
	// y -> L
	if ( i < lw ) {
	  A[lw] = x;
	  lw--;
	  x = y;
	  goto TLgMRL;
	}
	// left gap closed also
	A[N] = y;
	A[lw] = x;
        /*   L                            R
	   |----][----------------][---------|
	   N     i                j          M

	 */
	i--;
	j++;
	goto Finish;

 TLgMRL:
        /*   L                            R
	   |o----]---------+[-----][---------|
	   N     i        lw      j          M
           x -> L
	 */
	i++;
	y = A[i];
	// if ( y <= maxl )
	if ( compareXY(y, maxl) <= 0 ) 
	  goto TLgMRL;
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) {
	  A[i] = x;
	  x = y;
	  goto TLgMRR;
	}
	// y -> M
	if ( lw < i ) { // left gap closed also
	  A[N] = x;
	  i--;
	  j++;
	  goto Finish;
	}
	A[i] = x;
	x = y;
	goto TLgMRM;

 TLgMRR:
        /*   L                            R
	   |o----]---------+[-----][---------|
	   N     i        lw      j          M
           x -> R
	 */
	y = A[lw];
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) {
	  A[lw] = A[j];
	  lw--;
	  A[j] = y;
	  j--;
	  goto TLgMRR;
	}
	// if ( maxl < y )
	if ( compareXY(maxl, y) < 0 ) { // y -> M
	  lw--;
	  goto TLgMRR;
	}
	// y -> L
	if ( i < lw ) {
	  A[lw] = A[j];
	  lw--;
	  A[j] = x;
	  j--;
	  x = y;
	  goto TLgMRL;
	}
	// left gap closed i = lw
	A[N] = y;
	A[lw] = A[j];
	A[j] = x;
	i--;
	goto Finish;

 TLMgRM:
        /*   L                            R
	   |o----][---------]+-----[---------|
	   N      i          up    j         M
           x -> M
	 */
	y = A[up];
	// if ( y <= maxl )
	if ( compareXY(y, maxl) <= 0 ) {
	  A[up] = A[i];
	  up++;
	  A[i] = y;
	  i++;
	  goto TLMgRM;
	} 
	// if ( y < minr ) 
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMgRM;
	}
	// y -> R
	if ( up < j ) {
	  A[up] = x;
	  up++;
	  x = y;
	  goto TLMgRR;
	}
	// right gap closed
	i--;
	A[N] = A[i];
	A[i] = x;
	i--;
	goto Finish;

 TLMgRR:
        /*   L                            R
	   |o----][---------]+-----[---------|
	   N      i          up    j         M
           x -> R
	 */
	j--;
	y = A[j];
	// if ( minr <= y )
	if ( compareXY(minr, y) <= 0 ) 
	  goto TLMgRR;
	// if ( y <= maxl )
	if ( compareXY(y, maxl) <= 0 ) {
	  A[j] = x;
	  x = y;
	  goto TLMgRL;
	}
	// y -> M
	if ( up <= j ) {
	  A[j] = x;
	  x = y;
	  goto TLMgRM;
	}
	// right gap closed
	i--;
	A[N] = A[i];
	A[i] = A[j];
	i--;
	A[j] = x;
	goto Finish;

 TLMgRL:
        /*   L                            R
	   |o----][---------]+-----[---------|
	   N      i          up    j         M
           x -> L
	 */
	y = A[up];
	// if ( y <= maxl )
	if ( compareXY(y, maxl) <= 0 ) {
	  A[up] = A[i];
	  up++;
	  A[i] = y;
	  i++;
	  goto TLMgRL;
	}
	// if ( y < minr) 
	if ( compareXY(y, minr) < 0 ) {
	  up++;
	  goto TLMgRL;
	}
	// y -> R
	if ( up < j ) {
	  A[up] = A[i];
	  up++;
	  A[i] = x;
	  i++;
	  x = y;
	  goto TLMgRR;
	}
	i--;
	A[N] = x;
	goto Finish;
 Finish:
	// printf("cut3Pc Finish N: %i i: %i j: %i M: %i \n", N, i, j, M);
	  /*
	      |------]---------[----------------|
	      N      i         j                M

	      N < i < j < M
	      N <= x <= i  ==> A[x] <= maxl
	      i < x < j  ==> maxl < A[x] < minr
	      j <= x <= M ==> minr <= A[x] 
	  */
	/*	
	for (k = N; k <= i; k++) 
	  if ( compareXY(maxl, A[k]) < 0 ) printf("Err1 k: %i\n", k);
	for (k = i+1; k < j; k++) 
	  if ( compareXY(A[k], maxl) <= 0 ||
	       compareXY(minr, A[k]) <= 0 ) printf("Err2 k: %i\n", k);
	for (k = j; k <= M; k++)
	  if ( compareXY(A[k], minr) < 0 ) printf("Err3 k: %i\n", k);
	*/
	/*
	if ( i - N < j - i ) {
	  cut3Pc(N, i, depthLimit);
	  if ( j - i < M - j ) {
	    cut3Pc(i+1, j-1, depthLimit);
	    N = j;
	    goto Begin;
	  }
	  cut3Pc(j, M, depthLimit);
	  N = i+1; M = j-1;
	  goto Begin;
	}
	*/
	if ( i - N < j - i ) {
	  if ( j - i < M - j ) {
	    addTaskSynchronized(ll, newTask(j, M, depthLimit));
	    addTaskSynchronized(ll, newTask(i+1,j-1, depthLimit));
	  } else {
	    addTaskSynchronized(ll, newTask(i+1,j-1, depthLimit));
	    addTaskSynchronized(ll, newTask(j, M, depthLimit));
	  }
	  M = i;
	  goto Begin;
	}
	/*
	if ( j - i < M - j ) {
	  cut3Pc(i+1, j-1, depthLimit);
	  if ( i - N < M - j) {
	    cut3Pc(N, i, depthLimit);
	    N = j;
	    goto Begin;
	  }
	  cut3Pc(j, M, depthLimit);
	  M = i;
	  goto Begin;
	}
	*/
	if ( j - i < M - j ) {
	  if ( i - N < M - j) {
	    addTaskSynchronized(ll, newTask(j, M, depthLimit));
	    addTaskSynchronized(ll, newTask(N, i, depthLimit));
	  } else {
	    addTaskSynchronized(ll, newTask(N, i, depthLimit));
	    addTaskSynchronized(ll, newTask(j, M, depthLimit));
	  }
	  N = i+1;
	  M = j-1;
	  goto Begin;
	}
	/*
	cut3Pc(j, M, depthLimit);
	cut3Pc(i+1, j-1, depthLimit);
	M = i;
	goto Begin;
	*/
	addTaskSynchronized(ll, newTask(N, i, depthLimit));
	addTaskSynchronized(ll, newTask(i+1,j-1, depthLimit));
	N = j;
	goto Begin;
} // end cut3Pc


void heapSort();
void heapc(void **A, int N, int M) {
  // printf("heapc: %d %d\n", N, M);
  heapSort(&A[N], M-N+1);
} // end heapc

void heapify();
void siftDown();
void heapSort(void **a, int count) {
  // input:  an unordered array a of length count
  // first place a in max-heap order
  heapify(a, count);
  // in languages with zero-based arrays the children are 2*i+1 and 2*i+2
  int end = count-1; 
  while ( end > 0 ) {
    // (swap the root(maximum value) of the heap with 
    // the last element of the heap)
    // swap(a[end], a[0]);
    iswap(end, 0, a);
    // (decrease the size of the heap by one so that the 
    // previous max value will stay in its proper placement) 
    end = end - 1;
    // (put the heap back in max-heap order)
    siftDown(a, 0, end);
  }
} // end heapSort
         
void heapify(void **a, int count) {
  // (start is assigned the index in a of the last parent node)
  int start = (count - 2) / 2;
  while ( 0 <= start ) {
    // (sift down the node at index start to the proper place such 
    // that all nodes below the start index are in heap order)
    siftDown(a, start, count-1);
    start = start - 1;
  } // (after sifting down the root all nodes/elements are in heap order)
} // end heapify
 
void siftDown(void **a, int start, int end) {
  // input:  end represents the limit of how far down the heap to sift.
  int root = start;
  int child, swapi;
  // (While the root has at least one child)
  while ( root * 2 + 1 <= end ) {
    child = root * 2 + 1; // (root*2 + 1 points to the left child)
    swapi = root; // (keeps track of child to swap with)
    // (check if root is smaller than left child)
    // if ( a[swapi] < a[child] ) 
    if ( compareXY(a[swapi], a[child]) < 0 ) 
      swapi = child;
    // (check if right child exists, and if it's bigger 
    // than what we're currently swapping with)
    // if ( child+1 <= end && a[swapi] < a[child+1] )
    if ( child+1 <= end && compareXY(a[swapi],a[child+1]) < 0 )
      swapi = child + 1;
    // (check if we need to swap at all)
    if ( swapi != root ) {
      // swap(a[root], a[swapi]);
      iswap(root, swapi, a);
      root = swapi; // (repeat to continue sifting down the child now)
    } else return;
  }
} // end siftDown

void *myMallocSS(char* location, int size) {
  void *p = malloc(size);
  if ( 0 == p ) {
    fprintf(stderr, "malloc fails for: %s\n", location);
    // printf("Cannot allocate memory in: %s\n", location);
    exit(1);
  }
  return p;
} // end of myMalloc


  // To obtain the int n field from X: ((struct task *) X)->n
  // To obtain the int m field from X: ((struct task *) X)->m
  // To obtain the task next field from X: ((struct task *) X)->next
struct task {
  int n;
  int m;
  int dl;
  struct task *next;
};
int getN(struct task *t) { return ((struct task *) t)->n; }
int getM(struct task *t) { return ((struct task *) t)->m; }
int getDL(struct task *t) { return ((struct task *) t)->dl; }
struct task *getNext(struct task *t) { return ((struct task *) t)->next; }

void setN(struct task *t, int n) { ((struct task *) t)->n = n; }
void setM(struct task *t, int m) { ((struct task *) t)->m = m; }
void setDL(struct task *t, int dl) { ((struct task *) t)->dl = dl; }
void setNext(struct task *t, struct task* tn) { 
  ((struct task *) t)->next = tn; }
struct task *newTask(int N, int M, int depthLimit) {
  struct task *t = (struct task *) 
    myMallocSS("ParFourSort/ newTask()", sizeof (struct task));
  setN(t, N); setM(t, M); setDL(t, depthLimit); setNext(t, NULL);
  return t;
} // end newTask

struct stack {
  struct task *first;
  int size;
};
struct task *getFirst(struct stack *ll) { 
  return ((struct stack *) ll)->first; }
int getSize(struct stack *ll) { 
  return ((struct stack *) ll)->size; }
void setFirst(struct stack *ll, struct task *t) { 
  ((struct stack *) ll)->first = t; }
void setSize(struct stack *ll, int s) { 
  ((struct stack *) ll)->size = s; }
void incrementSize(struct stack *ll) { 
  setSize(ll, getSize(ll) + 1); }
void decrementSize(struct stack *ll) { 
  setSize(ll, getSize(ll) - 1); }
struct stack *newStack() {
  struct stack *ll = (struct stack *)
    myMallocSS("ParFourSort/ newStack()", sizeof (struct stack));
  setFirst(ll, NULL); setSize(ll, 0);
  return ll;
} // end newStack
int isEmpty(struct stack *ll) { 
  return ( NULL == getFirst(ll) ); 
} // end isEmpty
struct task *pop(struct stack *ll) {
  struct task *t = getFirst(ll);
  if ( NULL == t ) return NULL;
  setFirst(ll, getNext(t));
  decrementSize(ll);
  return t;
} // end pop
void push(struct stack *ll, struct task *t) {
  if ( !isEmpty(ll) ) setNext(t, getFirst(ll)); 
  setFirst(ll, t);
  incrementSize(ll);
} // end push


pthread_mutex_t condition_mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond2  = PTHREAD_COND_INITIALIZER;


void addTaskSynchronized(struct stack *ll, struct task *t) {
  /*
  printf("Thread#: %ld ", pthread_self());
  printf("addTask N: %d M: %d sleepingThreads: %d\n", 
	 getN(t), getM(t), sleepingThreads);
  */
  pthread_mutex_lock( &condition_mutex2 );
  push(ll, t);
  pthread_cond_signal( &condition_cond2 );
  pthread_mutex_unlock( &condition_mutex2 );
} // end addTaskSynchronized


void *sortThread(void *A) { // A-argument is NOT used
  /*
  int taskCnt = 0;
  printf("Thread number: %ld #sleepers %d\n", 
         pthread_self(), sleepingThreads);
  */
  struct task *t = NULL;
  for (;;) {
    pthread_mutex_lock( &condition_mutex2 );
       sleepingThreads++;
       while ( NULL == ( t = pop(ll) ) && 
	       sleepingThreads < NUMTHREADS ) {
	 pthread_cond_wait( &condition_cond2, &condition_mutex2 );
       }
       if ( NULL != t ) sleepingThreads--;
    pthread_mutex_unlock( &condition_mutex2 );
    if ( NULL == t ) { 
      pthread_mutex_lock( &condition_mutex2 );
      pthread_cond_signal( &condition_cond2 );
      pthread_mutex_unlock( &condition_mutex2 );
      break;
    }
    int n = getN(t);
    int m = getM(t);
    int depthLimit = getDL(t);
    free(t);
    // taskCnt++;
    cut3Pc(n, m, depthLimit);
  }
  // printf("Exit of Thread number: %ld taskCnt: %d\n", pthread_self(), taskCnt);
  return NULL;
} // end sortThread

int cut2SLimit = 2000;
void fivesort(void **AA, int size, 
	 int (*compar ) (const void *, const void * ),
	 int numberOfThreads) {
  /*
  // Set host & licence expiration date
  char* myHost = "ddc2";
  int year = 2010;
  int month = 11; // December = 11
  int day = 31;
  // show allowed host and licence expiration date
  if ( 0 == AA ) {
    printf("Licence expires: %d / %d / %d\n", year, month+1, day);
    printf("Host: %s\n", myHost);
    exit(0);
  }
  // Check whether licence expired
  time_t now = time(NULL);
  struct tm *laterx = localtime(&now);
  laterx->tm_year = year-1900; // = 2010
  laterx->tm_mon = month;
  laterx->tm_mday = day;
  time_t endTime = mktime(laterx);
  if ( endTime < now ) {
     printf(expiration);
     exit(1);
  }
  // Check whether this host is allowed
  int nameLng = 100;
  char hostName[nameLng];
  int out = gethostname(hostName, nameLng);
  if ( 0 != out ) {
    printf("*** Error: cannot get: Host name\n");
    exit(1);
  }
  out = strcmp(myHost, hostName);
  if ( 0 != out ) {
    printf("*** Error: not allowed host\n");
    exit(1);
  }
  */
  // Proceed !
  A = AA;
  compareXY = compar;
  if ( size <= cut2SLimit || numberOfThreads <= 0) {
    cut2(0, size-1);
  }
  // cut3P(0, size-1);
  sleepingThreads = 0;
  NUMTHREADS = numberOfThreads;
  // printf("Entering sortArray\n");
  ll = newStack();
  int depthLimit = 2 * floor(log(size));
  struct task *t = newTask(0, size-1, depthLimit);
  addTaskSynchronized(ll, t);

  pthread_t thread_id[NUMTHREADS];
  int i;
  int errcode;
  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_create(&thread_id[i], NULL, 
				sortThread, (void*) A) ) {
      errexit(errcode,"ParSort/ sortArray()/pthread_create");
    }
  }

  for ( i = 0; i < NUMTHREADS; i++ ) {
    if ( errcode=pthread_join(thread_id[i], NULL) ) {
      errexit(errcode,"ParSort/ sortArray()/pthread_join");
    }
  }
  free(ll);
  // printf("Exiting sortArray\n");

} // end fivesort
/* compile with: 
   gcc -c ParFiveSort.c
   which produces:
   ParFiveSort.o
   Compile & link with UseParFiveSort:
   gcc UseParFiveSort.c ParFiveSort.o
*/
