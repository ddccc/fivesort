/*
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

// File: c:/bsd/rigel/sort/ParFiveSort.c
// Date: Sat Dec 28 15:31:42 2013
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

const int cut2Limit = 127;
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
  // find minimum  ...
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
  // ... and put it at loc N
  if ( N != minimumPosition ) {
    A[minimumPosition] = A[N];
    A[N] = minimum;
  }
  // now push elements back to their proper location
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
	  dflgm(N, M, duplicate, quicksort0, depthLimit);
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

void dflgm();
// Cut2 does 2-partitioning with one pivot.
// It is included for comparisons.  
// Cut2 invokes dflgm when label invariants fail at the start
// cut2 is used as a best in class quicksort implementation 
// {with a defense against quadratic behavior due to duplicates}
void cut2c(int N, int M, int depthLimit) {
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
        int sixth = (L + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
        int e3 = (N+M) / 2; // The midpoint
        int e4 = e3 + sixth;
        int e2 = e3 - sixth;

        // Sort these elements using a 5-element sorting network
        void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
	void *t;
	// if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
	if ( 0 < compareXY(ae1, ae2) ) { t = ae1; ae1 = ae2; ae2 = t; } // 1-2
	if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
	if ( 0 < compareXY(ae1, ae3) ) { t = ae1; ae1 = ae3; ae3 = t; } // 1-3
	if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
	if ( 0 < compareXY(ae1, ae4) ) { t = ae1; ae1 = ae4; ae4 = t; } // 1-4
	if ( 0 < compareXY(ae3, ae4) ) { t = ae3; ae3 = ae4; ae4 = t; } // 3-4
	if ( 0 < compareXY(ae2, ae5) ) { t = ae2; ae2 = ae5; ae5 = t; } // 2-5
	if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
	if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
	// ... and reassign
	A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

	// Fix end points
	if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
	if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);

	register void *T = ae3; // pivot

	// check Left label invariant
	// if ( T <= A[N] || A[M] < T ) {
	if ( compareXY(T, A[N]) <= 0 || compareXY(A[M], T) < 0 ) {
	   // give up because cannot find a good pivot
	   // dflgm is a dutch flag type of algorithm
	   void cut2c();
	   dflgm(N, M, e3, cut2c, depthLimit);
	   return;
	 }

	register int I, J; // indices
	register void *AI, *AJ; // array values


	// initialize running indices
	I= N;
	J= M;

	// The left segment has elements < T
	// The right segment has elements >= T
  Left:
	// I = I + 1;
	// AI = A[I];
	// if (AI < T) goto Left;
	// if ( compareXY(AI,  T) < 0 ) goto Left;
	while ( compareXY(A[++I],  T) < 0 ); AI = A[I];
  Right:
	// J = J - 1;
	// AJ = A[J];
	// if ( T <= AJ ) goto Right;
	// if ( compareXY(T, AJ) <= 0 ) goto Right;
	while ( compareXY(T, A[--J]) <= 0 ); AJ = A[J];
	if ( I < J ) {
	  A[I] = AJ; A[J] = AI;
	  goto Left;
	}
	// Tail iteration
	if ( (I - N) < (M - J) ) { // smallest one first
	  cut2c(N, J, depthLimit);
	  N = I; 
	  goto Start;
	}
	cut2c(I, M, depthLimit);
	M = J;
	goto Start;
} // (*  OF cut2c; *) ... the brackets remind that this was Pascal code


// Dutch flag type function that initializes the middle segment in the middle
void dflgm(int N, int M, int pivotx, void (*cut)(), int depthLimit) {
  // printf("dflgm N %i M %i pivotx %i dl %i\n", N,M,pivotx,depthLimit);  
  /*
    Simple version of partitioning with: L/M/R
    L < pivot, M = pivot, R > pivot
   */
  register int i, j, lw, up; // indices
  register void* p3; // pivot
  register void* ai, *aj, *am;
  register int r; // comparison output 
  // int z; // for tracing

  /*
  if ( depthLimit <= 0 ) { // avoid quadradic explosion
    heapc(A, N, M);
    return;
  }
  int L = M - N;
  if ( L < 100 ) {
    // delegated small segments
    quicksort0c(N, M, depthLimit);
    return;
  }
  */

  i = N; j = M; lw = pivotx-1; up = pivotx+1;
    /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      
      N <= i < lw <= up < j <= M
      N <= x < i ==> A[x] < p3
      lw < x < up  ==> A[x] = p3
      j < x <= M & ==> p3 < A[x] 
    */
  p3 = A[pivotx]; // There IS a middle value somewhere:

 L0:
   /*
     |---)-----(----)-------(----|
     N   i     lw  up       j    M
   */

  // while ( A[i] < p3 ) i++;
  while ( (r = compareXY(A[i], p3)) < 0 ) i++;
  if ( lw < i ) {
    i--;
    if ( N < i ) goto leftClosed;
    i = N;
    goto leftEmpty;
  }
  ai = A[i]; // p3 <= ai
  // if ( p3 < ai ) goto LaiR; 
  if ( 0 < r ) goto LaiR; 
  goto LaiM;

 LaiR:
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      ai -> R
   */
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    A[i] = aj; A[j] = ai; 
    goto L1; }
  // aj -> M
  if ( j <= up ) {
    if ( j == M ) goto emptyRightaiR;
    j++; goto rightClosedaiR;
  }
  // up < j
  am = A[up];
  // if ( am < p3 ) { // am -> L
  if ( (r = compareXY(am, p3)) < 0 ) {
    A[i] = am; A[up++] = aj; A[j] = ai;
    goto L1; 
  }
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    A[up++] = aj; A[j--] = am; 
    if ( j < up ) { j++; goto rightClosedaiR; }
    // up <= j
    goto LaiR; // info loss ...
  }
  // am -> M
  up++;
  goto LaiR;

 LaiM:
   /*
      |---)-----(----)-------(----|
      N   i     lw  up       j    M
      ai -> M
   */
  // while ( N < lw && A[lw] == p3 ) lw--;
  while ( N < lw && (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( N == lw ) { goto leftEmpty; }
  // N < lw
  if ( i == lw ) { i--; goto leftClosed; }
  // i < lw
  am = A[lw];
  // if ( am < p3 ) { // am -> L
  if ( r < 0 ) {
    A[i++] = am; A[lw--] = ai;
    if ( lw < i ) { i--; goto leftClosed; }
    goto L0; // info loss
  }
  // am -> R
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    A[i] = aj; A[lw--] = ai; A[j] = am;
    goto L1;
  }
  // aj -> M
  if ( j < up ) {
    A[lw--] = aj; A[j] = am;
    goto rightClosedaiM;
  }
  // up <= j
  A[lw--] = aj; A[j--] = am;
  if ( j < up ) {
    j++; // right side closed
    if ( i == lw ) { // left side closed
      i--; goto done;
    }
    goto rightClosedaiM;
  }
  // up <= j
  if ( i == lw ) {
    i--; goto leftClosed;
  }
  goto LaiM; // info loss
  
 leftEmpty:
   /*
      |-------------)-------(----|
    N=i             up      j    M
   */
  // while ( up <= M && A[up] == p3 ) up++;
  while ( up <= M && (r = compareXY(A[up], p3)) == 0 ) up++;
  if ( M < up ) return; // !!
  if ( j < up ) { j++; goto done; }
  // up <= j
  am = A[up];
  // if ( am < p3) { // am -> L
  if ( r < 0 ) {
    A[up++] = A[N]; A[N] = am; 
    if ( j < up ) { j++; goto done; }
    // up <= j
    goto leftClosed;
  }
  // am -> R
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  if ( j < up ) { j++; goto done; }
  // up < j
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    A[j--] = am; A[up++] = A[N]; A[N] = aj;
    if ( j < up ) { j++; goto done; }
    goto leftClosed;
  }
  // aj -> M
  A[j--] = am; A[up++] = aj;
  if ( j < up ) { j++; goto done; }
  goto leftEmpty;
  
 emptyRightaiR:
   /*
      |---)-----(---------------|
      N   i     lw              j=M
      ai -> R
   */
  // while ( A[lw] == p3 ) lw--;
  while ( (r = compareXY(A[lw], p3)) == 0 ) lw--;
  am = A[lw];
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    if ( i == lw ) {
      A[i] = A[M]; A[M] = ai; i--; goto done;
    }
    // i < lw
    A[lw--] = A[M]; A[M] = am; goto rightClosedaiR;
  }
  // am -> L
  A[i++] = am; A[lw--] = A[M]; A[M] = ai;
  if ( lw < i ) { i--; goto done; }
  goto rightClosed;

  L1: 
  /*
    |---]-----(----)-------[----|
    N   i     lw  up       j    M
  */
  // while ( A[++i] < p3 );
  while ( (r = compareXY(A[++i], p3)) < 0 );
  if ( lw < i ) { i--; j--; goto leftClosed; }
  // i <= lw
  ai = A[i];
  // if ( p3 < ai ) { // ai -> R
  if ( 0 < r ) {
  L1Repeat:
    // while ( p3 < A[--j] );
    while ( (r = compareXY(p3, A[--j])) < 0 );
    if ( j < up ) { j++; goto rightClosedaiR; }
    // up <= j
    aj = A[j];
    // if ( aj < p3 ) { // aj -> L
    if ( 0 < r ) {
      A[i] = aj; A[j] = ai; 
      goto L1;
    }
    // aj -> M
    // while ( A[up] == p3 ) up++;
    while ( (r = compareXY(A[up], p3)) == 0 ) up++;
    if ( j < up ) { j++; goto rightClosedaiR; }
    // up <= j
    am = A[up];
    // if ( am < p3 ) { // am -> L
    if ( r < 0 ) {
      A[i] = am; A[up++] = aj; A[j] = ai;
      if ( j < up ) { i++; goto rightClosed; }
      goto L1;
    }
    // am -> R
    A[up++] = aj; A[j] = am;
    if ( j < up ) { goto rightClosedaiR; }
    // up <= j
    goto L1Repeat;
  }
  // ai -> M
 L1Repeat2:
  // while ( A[lw] == p3 ) lw--;
  while ( (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( lw < i ) { i--; j--; goto leftClosed; }
  // i < lw
  am = A[lw]; 
  // if ( am < p3 ) { // am -> L
  if ( r < 0 ) {
    A[i] = am; A[lw--] = ai;
    if ( lw < i ) { i--; j--; goto leftClosed; } 
    goto L1;
  }
  // am -> R
  // while ( p3 < A[--j] );
  while ( (r = compareXY(p3, A[--j])) < 0 );
  if ( j < up ) {
    A[lw--] = A[j]; A[j] = am; goto rightClosedaiM;
  }
  // up <= j
  aj = A[j];
  // if ( aj < p3 ) { // aj -> L
  if  ( 0 < r ) {
    A[i] = aj; A[lw--] = ai; A[j] = am;
    if ( lw < i ) { j--; goto leftClosed; }
    goto L1;
  }
  // aj -> M
  A[lw--] = aj; A[j] = am; 
  goto L1Repeat2;

 leftClosed: 
   /* 
      |---]----------)-------(----|
      N   i          up      j    M
   */
  // while ( p3 < A[j] ) j--;
  while ( (r = compareXY(p3, A[j])) < 0 ) j--;
  if ( j < up ) { j++; goto done; }
  // up <= j
  aj = A[j]; // aj <= p3
  // if ( aj < p3 ) { // aj -> L
  if ( 0 < r ) {
    repeatM:
    // while ( A[up] == p3 ) up++;
    while ( (r = compareXY(A[up], p3)) == 0 ) up++;
    am = A[up];
    // if ( p3 < am ) { // am -> R
    if ( 0 < r ) {
      A[j--] = am; A[up++] = A[++i]; A[i] = aj;
      if ( j < up ) { j++; goto done; }
      goto leftClosed;
    }
    // am -> L
    if ( up == j ) { A[j++] = A[++i]; A[i] = am; goto done; }  
    // up < j
    A[up++] = A[++i]; A[i] = am;
    goto repeatM;
  }
  // aj -> M
  repeatM2:
  // while ( up <= j && A[up] == p3 ) up++;
  while ( up <= j && (r = compareXY(A[up], p3)) == 0 ) up++;
  if ( j <= up ) { j++; goto done; }
  // up < j
  am = A[up];
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    A[j--] = am; A[up++] = aj;
    if ( j < up ) { j++; goto done; }
    goto leftClosed;
  }
  // am -> L
  A[up++] = A[++i]; A[i] = am; 
  goto repeatM2;

 rightClosed:
   /*
      |---)-----(-----------[----|
      N   i    lw           j    M
   */
  // while ( A[i] < p3 ) i++;
  while ( (r = compareXY(A[i], p3)) < 0 ) i++;
  if ( lw < i ) { i--; goto done; }
  // i <= lw
  ai = A[i]; // p3 <= ai
  // if ( p3 < ai ) { // ai -> R
  if ( 0 < r ) {
    goto rightClosedaiR;
  }
  // ai -> M
  // goto rightClosedaiM;

 rightClosedaiM:
  // while ( i <= lw && A[lw] == p3 ) lw--;
  while ( i <= lw && (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( lw < i ) { i--; goto done; }
  // i <= lw
  am = A[lw];
  // if ( p3 < am ) { // am -> R
  if ( 0 < r ) {
    A[lw--] = A[--j]; A[j] = am; 
    if ( lw < i ) { i--; goto done; }
    goto rightClosedaiM;
  }
  // am -> L
  A[i++] = am; A[lw--] = ai;
  if ( lw < i ) { i--; goto done; }
  goto rightClosed;

 rightClosedaiR: 
   /*
      |---)-----(-----------[----|
      N   i    lw           j    M
      ai -> R
   */
  // while ( A[lw] == p3 ) lw--;
  while ( (r = compareXY(A[lw], p3)) == 0 ) lw--;
  if ( i == lw ) {
    A[i--] = A[--j]; A[j] = ai; goto done;
  }
  // i < lw
  am = A[lw];
  // if ( am < p3 ) { // am -> L
  if ( r < 0 ) {
    A[i++] = am; A[lw--] = A[--j]; A[j] = ai;
    if ( lw < i ) { i--; goto done; }
    // i <= lw
    goto rightClosed;
  }
  // am -> R
  A[lw--] = A[--j]; A[j] = am; goto rightClosedaiR;
  
 done: 
    /*
      |---]---------[---------|
      N   i         j         M
    */
  // printf("done dflgm N %i i %i j %i M %i\n", N,i,j,M);

    /*
      for ( z = N; z <= i; z++ )
	// if ( p3 <= A[z] ) {
        if ( compareXY(p3, A[z]) <= 0 ) {
	  printf("doneL z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = i+1; z < j; z++ )
	// if ( p3 != A[z] ) {
	if ( compareXY(p3, A[z]) != 0 ) {
	  printf("doneM z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = j; z <= M ; z++ )
	// if ( A[z] <= p3 ) {
	if ( compareXY(A[z], p3) <= 0 ) {
	  printf("doneR z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      */
    if ( i - N  < M - j ) {
      (*cut)(N, i, depthLimit);
      (*cut)(j, M, depthLimit);
      return;
    }
    (*cut)(A, j, M, depthLimit);
    (*cut)(A, N, i, depthLimit);
} // end dflgm


struct stack *ll;
struct task *newTask();
void addTaskSynchronized();


void tpsc();
// tps is the header function for the three partition sorter tpsc
void tps(int N, int M) {
  int L = M - N;
  if ( L < cut2Limit ) { 
    quicksort0(N, M);
    return;
  }
  int depthLimit = 2 * floor(log(L));
  tpsc(N, M, depthLimit);
} // end tps

void tpsc(int N, int M, int depthLimit) {  
  // int z; // for tracing
  register int i, j, up, lw; // indices
  register void *ai, *aj, *am; // array values
  void *pl, *pr; // pivots

 // A 3d recursive call is avoided by jumping back to Start.  
 Start:
  // printf("tpsc N %i M % i dl %i\n", N,M,depthLimit);
  if ( depthLimit <= 0 ) { // prevent quadradic explosion
    heapc(A, N, M);
    return;
  }
  int L = M - N;
  if ( L < cut2Limit ) {
    quicksort0c(N, M, depthLimit);
    return;
  }
  depthLimit--;
        int sixth = (L + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
        int e3 = (N+M) / 2; // The midpoint
        int e4 = e3 + sixth;
        int e2 = e3 - sixth;
        // Sort these elements using a 5-element sorting network
        void *ae1 = A[e1], *ae2 = A[e2], *ae3 = A[e3], *ae4 = A[e4], *ae5 = A[e5];
	void *t;
	// if (ae1 > ae2) { t = ae1; ae1 = ae2; ae2 = t; }
	if ( 0 < compareXY(ae1, ae2) ) { t = ae1; ae1 = ae2; ae2 = t; } // 1-2
	if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
	if ( 0 < compareXY(ae1, ae3) ) { t = ae1; ae1 = ae3; ae3 = t; } // 1-3
	if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
	if ( 0 < compareXY(ae1, ae4) ) { t = ae1; ae1 = ae4; ae4 = t; } // 1-4
	if ( 0 < compareXY(ae3, ae4) ) { t = ae3; ae3 = ae4; ae4 = t; } // 3-4
	if ( 0 < compareXY(ae2, ae5) ) { t = ae2; ae2 = ae5; ae5 = t; } // 2-5
	if ( 0 < compareXY(ae2, ae3) ) { t = ae2; ae2 = ae3; ae3 = t; } // 2-3
	if ( 0 < compareXY(ae4, ae5) ) { t = ae4; ae4 = ae5; ae5 = t; } // 4-5
	// ... and reassign
	A[e1] = ae1; A[e2] = ae2; A[e3] = ae3; A[e4] = ae4; A[e5] = ae5;

	// Fix end points
	if ( compareXY(ae1, A[N]) < 0 ) iswap(N, e1, A);
	if ( compareXY(A[M], ae5) < 0 ) iswap(M, e5, A);

	void tpsc();
	// if ( ae2 == ae3 || ae3 == ae4 ) {
	if ( compareXY(ae2, ae3) == 0 || compareXY(ae3, ae4) == 0 ) {
	  dflgm(N, M, e3, tpsc, depthLimit);
	  return;
	}

	pl = A[e2]; pr = A[e4];
	// if ( pl <= A[N] || A[M] <= pr ) {
	if ( compareXY(pl, A[N]) <= 0 || compareXY(A[M], pr) <= 0 ) {
	  // ascertain that the corners are not empty
	  dflgm(N, M, e3, tpsc, depthLimit);
	  return;
	}

	// initialize running indices
	i = N+1; j = M-1; 
	lw = e3; 
	// iswap(e3, lw, A);
	up = lw+1; lw--;

	// while ( A[i] < pl ) i++;
	while ( compareXY(A[i], pl) < 0 ) i++;
	// while ( pr < A[j] ) j--;
	while ( compareXY(pr, A[j]) < 0 ) j--;

	/* 
	  |)----------(--)-------------(|
	 N i         lw  up            j M
	  N <= x < i -> A[x] < pl
	  lw < x < lup -> pl <= A[x] <= pr
	  j < x <= M -> pr < A[x]
	*/
 again:
	while ( i <= lw ) {
	  ai = A[i];
	  // if ( ai < pl ) { i++; continue; }
	  if ( compareXY(ai, pl) < 0 ) { i++; continue; }
	  // if ( pr < ai) { // ai -> R
	  if ( compareXY(pr, ai) < 0 ) {
	    // while ( pr < A[j] ) j--;
	    while( compareXY(pr, A[j]) < 0 ) j--;
	    aj = A[j]; // aj <= pr
	    // if ( aj < pl ) { // aj -> L
	    if ( compareXY(aj, pl) < 0 ) {
	      A[i++] = aj; A[j--] = ai; 
	      if ( j < up ) { j++;
		if ( lw < i ) { i--; goto done; }
		goto rightClosed;
	      }
	      continue;
	    }
	    // aj -> M
	    if ( j < up ) { // right gap closed
	      j++; goto rightClosedAIR;
	    } // up <= j
	    goto AIRAJM;
	  } 
	  // ai -> M
	repeatM2:
	  if ( lw < i ) { i--; goto leftClosed; }
	  // i <= lw
	  am = A[lw];
	  // if ( am < pl ) { // am -> L
	  if ( compareXY(am, pl) < 0 ) {
	    A[i++] = am; A[lw--] = ai; 
	    if ( lw < i ) { i--; goto leftClosed; }
	    continue;
	  }
	  // if ( pr < am ) { // am -> R
	  if ( compareXY(pr, am) < 0 ) {
	  repeatR:
	    aj = A[j]; 
	    // if ( pr < aj ) { j--; goto repeatR; }
	    if ( compareXY(pr, aj) < 0 ) { j--; // aj -> R
	      if ( j < up ) { // right closed
		A[lw--] = A[j]; A[j] = am;
		if ( i == lw ) { i--; goto done; }
		goto rightClosedAIM;
	      } 
	      goto repeatR; 
	    }
	    // if ( aj < pl ) { // aj -> L
	    if ( compareXY(aj, pl) < 0 ) {
	      A[i++] = aj; A[lw--] = ai; A[j--] = am; 
	      if ( j < up ) { j++; // right closed
		if ( lw < i ) { // left closed
		  i--; goto done;
		}
		goto rightClosed;
	      }
	      continue;
	    }
	    // aj -> M
	    if ( j < up ) { // right closed
	      j++; 
	      goto rightClosedAIM;
	    }
	    A[lw--] = aj; A[j--] = am; 
	    if ( j < up ) { j++; // right closed
	      if ( lw < i ) { i--; goto done; }
	      goto rightClosed;
	    }
	    goto repeatM2;
	  }
	  // am -> M
	  if ( i == lw ) { i--; goto leftClosed; }
	  lw--; goto repeatM2;
	}
	// left gap closed
	i--;
	goto leftClosed;

 AIRAJM:
	/* 
	  |)----------(--)-------------(|
	 N i         lw  up            j M
	 ai -> R aj -> M 
	*/
	am = A[lw];
	// if ( am < pl ) { // am -> L
	if ( compareXY(am, pl) < 0 ) {
	  A[i++] = am; A[lw--] = aj; A[j--] = ai; 
	  if ( j < up ) { j++; // right closed
	    if ( lw < i ) { // left closed
	      i--; goto done;
	    }
	    goto rightClosed;
	  }
	  if ( lw < i ) { i--; goto leftClosed; }
	  goto again; 
	}
	// if ( am <= pr ) { // am -> M
	if ( compareXY(am, pr) <= 0 ) {
	  lw--; goto AIRAJM;
	}
	// am -> R
	if ( i == lw ) {
	    A[j--] = ai; A[i--] = aj; 
	    if ( j < up ) { j++; goto done; }
	    goto leftClosed;
	}
	// i < lw
	A[lw--] = aj; A[j--] = am; 
	if ( j < up ) { // right closed
	  j++; goto rightClosedAIR;
	}
	goto AIR;
	  
 AJM:
	/* 
	  |)----------(--)-------------(|
	 N i         lw  up            j M
	 aj -> M
	*/
	am = A[lw];
	// if ( pr < am ) { // am -> R
	if ( compareXY(pr, am) < 0 ) {
	  A[lw--] = aj; A[j--] = am; 
	  if ( j < up ) { // right closed
	    j++; goto rightClosed;
	  }
	  goto again;
	}
	// if ( pl <= am ) { // am -> M
	if ( compareXY(pl, am) <= 0 ) {
	  lw--; 
	  if ( lw < i ) { // left closed
	    i--; goto repeatM3;
	  }
	  goto AJM;
	}
	// aj -> M and am -> L
      repeatL:
	ai = A[i];
	// if ( ai < pl ) { i++; goto repeatL; }
	if ( compareXY(ai, pl) < 0 ) { i++; goto repeatL; }
	// if ( pr < ai ) { // ai -> R
	if ( compareXY(pr, ai) < 0 ) {
	  A[i++] = am; A[lw--] = aj; A[j--] = ai;
	  if ( j < up ) { j++; // right closed
	    if ( lw < i ) { i--; goto done; }
	    goto rightClosed;
	  }
	  if ( lw < i ) { i--; goto leftClosed; } 
	  goto again;
	}
	// ai -> M
	A[i++] = am; A[lw--] = ai; 
	if ( lw < i ) { i--; goto repeatM3; } 
	goto AJM;

 AIR:
	/* 
	  |)----------(--)-------------(|
	 N i         lw  up            j M
	 ai -> R
	*/
	aj = A[j];
	// if ( pr < aj ) { j--; goto AIR; }
	if ( compareXY(pr, aj) < 0 ) { j--; goto AIR; }
	// if ( aj < pl ) {
	if ( compareXY(aj, pl) < 0 ) {
	  A[i++] = aj; A[j--] = ai;
	  if ( j < up ) { j++; // right closed
	    if ( lw < i ) { i--; goto done; }
	    goto rightClosed;
	  }
	  goto again;
	}
	// aj -> M
	if ( j < up ) { j++; goto rightClosedAIR; }
	goto AIRAJM;

 leftClosed:
	  /* 
	  |--]------------)-------------(|
	 N   i            up            j M
	  */
	aj = A[j];
	// if ( pr < aj ) { j--; goto leftClosed; }
	if ( compareXY(pr, aj) < 0 ) { j--; goto leftClosed; }
	// if ( aj < pl ) { // aj -> L
	if ( compareXY(aj, pl) < 0 ) {
	repeatM:
	  am = A[up];
	  // if ( pr < am ) { // am -> R
	  if ( compareXY(pr, am) < 0 ) {
	    A[j--] = am; A[up++] = A[++i]; A[i] = aj; 
	    if ( j < up ) { j++; goto done; }
	    goto leftClosed;
	  }
	  // if ( pl <= am ) { // am -> M
	  if ( compareXY(pl, am) <= 0 ) {
	    up++; goto repeatM;
	  }
	  // am -> L
	  if ( up == j ) {
	    A[j++] = A[++i]; A[i] = aj; 
	    goto done;
	  }
	  // up < j
	  A[up++] = A[++i]; A[i] = am;  
	  goto repeatM;
	}

	// aj -> M
	if ( j < up ) { j++; goto done; }
	// up <= j
      repeatM3:
	am = A[up];
	// if ( pr < am ) { // am -> R
	if ( compareXY(pr, am) < 0 ) {
	  A[up++] = aj; A[j--] = am; 
	  if ( j < up ) { j++; goto done; }
	  goto leftClosed;
	}
	// if ( am < pl ) { // am -> L
	if ( compareXY(am, pl) < 0 ) {
	  A[up++] = A[++i]; A[i] = am;
	  goto repeatM3;
	}
	// am -> M
	if ( j <= up ) { j++; goto done; }
	up++; goto repeatM3;
	
 rightClosedAIR:
	/* 
	   |-)----------(--[-------------|
	   N i         lw  j             M
	   ai -> R
	*/
	am = A[lw];
	// if ( am < pl ) { // am -> L
	if ( compareXY(am, pl) < 0 ) {
	  A[i++] = am; A[lw--] = A[--j]; A[j] = ai;
	  if ( lw < i ) { i--; goto done; }
	  goto rightClosed;
	}
	// if ( am <= pr ) { lw--; goto rightClosed; }
	if ( compareXY(am, pr) <= 0 ) { lw--; goto rightClosedAIR; }
	// am -> R
	if ( i == lw ) {
	  A[i--] = A[--j]; A[j] = ai; 
	  goto done;
	}
	// i < lw
	A[lw--] = A[--j]; A[j] = am;
	goto rightClosedAIR;

 rightClosedAIM:
	/* 
	   |-)----------(--[-------------|
	   N i         lw  j             M
	   ai -> M
	*/
	am = A[lw];
	// if ( am < pl ) { // am -> L
	if ( compareXY(am, pl) < 0 ) {
	  A[i++] = am; A[lw--] = ai;
	  if ( lw < i ) { i--; goto done; }
	  goto rightClosed;
	}
	// if ( pr < am ) { // am -> R
	if ( compareXY(pr, am) < 0 ) {
	  A[lw--] = A[--j]; A[j] = am; 
	  goto rightClosedAIM;
	}
	// am -> M
	if ( i == lw ) { i--; goto done; }
	lw--; 
	goto rightClosedAIM;

 rightClosed:
	/* 
	   |-)----------(--[-------------|
	   N i         lw  j             M
	*/
	// while ( A[i] < pl ) i++;
	while ( compareXY(A[i], pl) < 0 ) i++;
	ai = A[i];
	// if ( pr < ai ) {
	if ( compareXY(pr, ai) < 0 ) goto rightClosedAIR;
	// ai -> M
	if ( lw < i ) { i--; goto done;	}
	goto rightClosedAIM;

 done:
    /*
      |---]---------[---------|
      N   i         j         M
    */
    // printf("tpsc done N %i M % i dl %i\n", N,M,depthLimit);
    /*
      for ( z = N; z <= i; z++ )
	//	if ( pl <= A[z] ) {
	if ( compareXY(pl, A[z]) <= 0 ) {
	  printf("doneL z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = i+1; z < j; z++ )
	//	if ( A[z] < pl || pr < A[z] ) {
	if ( compareXY(A[z], pl) < 0 || pr < A[z] ) {
	  printf("doneM z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
      for ( z = j; z <= M ; z++ )
	//	if ( A[z] <= pr ) {
	if ( compareXY(A[z], pr) <= 0 ) {
	  printf("doneR z %i\n", z);
	  printf("N %i i %i lw %i up %i j %i M %i\n", N,i,lw,up,j,M);
	  exit(0);
	}
    */
	// tpsc(A, N, i, depthLimit);
	// tpsc(A, i+1, j-1, depthLimit);
	// tpsc(A, j, M, depthLimit);
	/*
      if ( i-N < j-i ) {
	tpsc(N, i, depthLimit);
	if ( j-i < M-j ) {
	   tpsc(i+1, j-1, depthLimit);
	   N = j; goto Start;
	   // (*cut)(A, j, M, depthLimit);
	   // return;
	}
	tpsc(j, M, depthLimit);
	N = i+1; M = j-1; goto Start;
	// (*cut)(A, i+1, j-1, depthLimit);
	// return;
      }
      tpsc(i+1, j-1, depthLimit);
      if ( i-N < M-j ) {
	tpsc(N, i, depthLimit);
	N = j; goto Start;
	// (*cut)(A, j, M, depthLimit);
	// return;
      }
      tpsc(j, M, depthLimit);
      M = i; goto Start;
      // (*cut)(A, N, i, depthLimit);
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
	  goto Start;
	}
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
	  goto Start;
	}
	addTaskSynchronized(ll, newTask(N, i, depthLimit));
	addTaskSynchronized(ll, newTask(i+1,j-1, depthLimit));
	N = j;
	goto Start;
} // end tpsc


void heapSort();
void heapc(void **A, int N, int M) {
  // printf("heapc: %d %d\n", N, M);
  if ( M <= N ) return;
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
    tpsc(n, m, depthLimit);
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
    tps(0, size-1);
  }
  // tps(0, size-1);
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
