// c:/bsd/rigel/sort/C2fpsort
// Date: Fri Jan 31 13:32:12 2014, 2017
// (C) OntoOO/ Dennis de Champeaux

// ***** This member is not used any longer Mon Aug 30 09:50:36 2021 ***


static const int cut2fpLimit = 2000;

// calculate the median of 3
static int medfp(void **A, int a, int b, int c,
	int (*compareXY ) (const void *, const void * ) ) {
  return
    compareXY( A[a], A[b] ) < 0 ?
    ( compareXY( A[b], A[c] ) < 0 ? b : compareXY( A[a], A[c] ) < 0 ? c : a)
    : compareXY( A[b], A[c] ) > 0 ? b : compareXY( A[a], A[c] ) > 0 ? c : a;
} // end medfp

static void cut2fpc();
// cut2fp is the parallel version of cut2f
void cut2fp(void **A, int N, int M, int (*compare)()) { 
  // printf("cut2fp %d %d %d \n", N, M, M-N;
  int L = M - N;
  if ( L < cut2fpLimit ) { 
    cut2f(A, N, M, compare);
    return;
  }
  int depthLimit = 2.9 * floor(log(L));
  cut2fpc(A, N, M, depthLimit, compare);
} // end cut2fp

// Cut2fpc does 2-partitioning with one pivot.
// Cut2fpc invokes cut2fc when duplicates are found.
void cut2fpc(void **A, int N, int M, int depthLimit, int (*compareXY)()) {
  int L;
  Start:
  // printf("cut2fpc %d %d %d \n", N, M, M-N);
  if ( depthLimit <= 0 ) {
    heapc(A, N, M, compareXY);
    return;
  }
  L = M - N +1;
  if ( L < cut2fpLimit ) { 
    cut2fc(A, N, M, depthLimit, compareXY);
    return;
  }
  depthLimit--;

    int p0 = N + (L>>1); // N + L/2;
    int pn = N;
    int pm = M;
    int d = (L-2)>>3; // L/8;
    pn = medfp(A, pn, pn + d, pn + 2 * d, compareXY);
    p0 = medfp(A, p0 - d, p0, p0 + d, compareXY);
    pm = medfp(A, pm - 2 * d, pm - d, pm, compareXY);
    p0 = medfp(A, pn, p0, pm, compareXY);
    iswap(N, p0, A); // ... and is put in first position

  register void *T = A[N];  // pivot
  register int I, J; // indices
  register void *AI, *AJ; // array values
  // int k; // tracing
    // 1st round of partitioning
	// The left segment has elements <= T
	// The right segment has elements > T
    /*
	  |----------]-------------[-----------|
	  N   <=T    I             J   >T      M   
    */

    J = M+1;
    while ( compareXY(T, A[--J]) < 0 );
    if ( N == J ) { 
      cut2c(A, N, M, depthLimit, compareXY);
      return;
    }
    AJ = A[J]; // A[J] <= T
    I = N+1;
    if (J < M ) {
      while ( compareXY(A[I], T) <= 0 ) I++;
    }
    else { // J = M
      if ( compareXY(T, A[M]) == 0 ) { // bail out
	cut2c(A, N, M, depthLimit, compareXY);
	return;
      }
      while ( I < J && compareXY(A[I], T) <= 0 ) { I++; }
      if ( M == I ) { // all elements are <= T, suspect bad input
	cut2c(A, N, M, depthLimit, compareXY);
	return;
      }
    }

    if ( I < J ) { // swap
      A[J] = A[I]; A[I] = AJ;
      if ( I+1 == J ) { J--; I++; goto Skip; }
      goto Left;
    }
    if (I == J+1 ) goto Skip;
    // I = J
     I++; 
     goto Skip;


     // The left segment has elements < T
     // The right segment has elements >= T
     // Proceed with fast loops
  Left:
	while ( compareXY(A[++I], T) <= 0 ); 
	if ( J < I ) goto Skip;
	AI = A[I];
	while ( compareXY(T, A[--J]) < 0 ); 
	AJ = A[J];
	if ( I < J ) { // swap
	  A[I] = AJ; A[J] = AI;
	  goto Left;
	}
 Skip:
	// Tail iteration
	if ( (I - N) < (M - J) ) { // smallest one first
	  // cut2fc(A, N, J, depthLimit, compareXY);
	  // N = I; 
	  addTaskSynchronized(ll, newTask(A, I, M, depthLimit, compareXY));
	  M = J;
	  goto Start;
	}
	// cut2fc(A, I, M, depthLimit, compareXY);
	// M = J;
	addTaskSynchronized(ll, newTask(A, N, J, depthLimit, compareXY));
	N = I;
	goto Start;

} // end of cut2fpc
// (*  OF cut2; *) the brackets remind that this was once Pascal code
