// c:/bsd/rigel/sort/Qusort
// Date: Fri Jan 31 13:32:12 2014/ Tue May 19 15:02:00 2015, 2017
// (C) OntoOO/ Dennis de Champeaux

#include "Isort" // insertionsort member
#include "Hsort" // heapsort member

// calculate the median of 3
int med(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end med


void quicksort0c();
// Quicksort function for invoking quicksort0c.
void quicksort0(void **A, int N, int M, int (*compare)()) {
  int L = M - N;
  if ( L <= 0 ) return;
  // printf("quicksort0 %d %d \n", N, M);
  if ( L <= 7 ) { 
    insertionsort(A, N, M, compare);
    return;
  }
  int depthLimit = 2.5 * floor(log(L));
  quicksort0c(A, N, M, depthLimit, compare);
} // end quicksort0

void dflgm();

// Quicksort equipped with a defense against quadratic explosion;
// calling heapsort if depthlimit exhausted
void quicksort0c(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
  // printf("Enter quicksort0c N: %d M: %d %d\n", N, M, depthLimit);
  while ( N < M ) {
    int L = M - N;
    if ( L <= 7 ) {
      insertionsort(A, N, M, compareXY);
      return;
    }
    if ( depthLimit <= 0 ) {
      heapc(A, N, M, compareXY);
      return;
    }
    depthLimit--;

    // 7 < L
    int p0 = N + (L>>1); // N + L/2;
    int pn = N;
    int pm = M;
    if ( 40 < L ) {
      int d = L>>3; // L/8;
      pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
      p0 = med(A, p0 - d, p0, p0 + d, compareXY);
      pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
 
    }
    p0 = med(A, pn, p0, pm, compareXY);
    /* optional check when inputs have many equal elements
    if ( compareXY(A[N], A[M]) == 0 ) {
      dflgm(N, M, p0, quicksort0c, depthLimit,, compareXY);
      return;
    } */
    // p0 is index to 'best' pivot ...
    
    iswap(N, p0, A); // ... and is put in first position

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
	  |-------o---------------(--------|
          N       i               j        M
	  N <= i < j <= M
          N <= k < i -> A[k] <= p    
          N < j < k <= M -> p < A[k]
	  A[N] <= p
          N < i -> p < A[i]
          p + A[N,i) + A(i,j] + A(j,M] is a permutation of the input array
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
	  |-------o---------------(--------|
          N       i               j        M
	  N = i = j < M or N < i & = i-1 <= j <= M or N <= i < j <= M
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
      if ( j <= i ) {
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
      }
      // i < j  & p < ai = A[i] 
      A[j] = ai;
      j--;
      /*
	  |--------o--------------[--------|
          N        i              j        M
	  N < i <= j <= M
          N <= k < i -> A[k] <= p    
          j <= k <= M -> p < A[k]
	  A[N] <= p
          N < i -> p < ai = A[i]
	  p + A[N,i) + A(i,M] is a permutation of the input array
      */
    } // end of while( i < j )

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
      if ( N < ia ) quicksort0c(A, N, ia, depthLimit, compareXY);  
      N = ib; 
    } else { 
      if ( ib < M ) quicksort0c(A, ib, M, depthLimit, compareXY);  
      M = ia; 
    }
  }
} // end of quicksort0c
