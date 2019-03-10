// File: c:/bsd/rigel/sort/FiveSort.c
// Date: Sun Dec 24 20:19:56 2017
/* This file has the source of the algorithms that make up FiveSort
   headed by fivesort
*/

// const int cut3Limit = 1900; // 4.18122e+08 clocktime 9587
const int cut3Limit = 1875; //    4.18155e+08 clocktime 9531
// const int cut3Limit = 1850; // 4.18192e+08 clocktime 9537
// const int cut3Limit = 1800; // 4.18257e+08 clocktime 9537
// const int cut3Limit = 1750; // 4.18327e+08 clocktime 9553


// Here more global entities used throughout
// int (*compareXY)();
// void **A;

/*
#include "Isort.c"
#include "Hsort.c"
#include "Qusort.c"
#include "Dsort.c"
#include "C2sort.c" 
*/

void tpsc();
// tps is the header function for the three partition sorter tpsc
void tps(void **A, int N, int M, int (*compareXY)() ) {
  int L = M - N;
  if ( L < cut3Limit ) { 
    cut2(A, N, M, compareXY);
    return;
  }
  int depthLimit = 2.5 * floor(log(L));
  tpsc(A, N, M, depthLimit, compareXY);
} // end tps

void tpsc(void **A, int N, int M, int depthLimit, int (*compareXY)()) {  
  // int z; // for tracing
  register int i, j, up, lw; // indices
  register void *ai, *aj, *am; // array values
  void *pl, *pr; // pivots

 // A 3d recursive call is avoided by jumping back to Start.  
 Start:
  // printf("tpsc N %i M % i dl %i\n", N,M,depthLimit);
  if ( depthLimit <= 0 ) { // prevent quadradic explosion
    heapc(A, N, M, compareXY);
    return;
  }
  int L = M - N;
  if ( L < cut3Limit ) {
    cut2c(A, N, M, depthLimit, compareXY);
    return;
  }
  depthLimit--;


  // const int small = 4450; // 4.19348e+08 clocktime 27587 27634
  const int small = 4400; // 4.19343e+08 clocktime 27581
  // const int small = 4350; // 4.19337e+08 clocktime 27640


  if ( L < small ) { // use 5 elements for sampling
        int sixth = (L + 1) / 6;
        int e1 = N  + sixth;
        int e5 = M - sixth;
	int e3 = N + L/2; // The midpoint
        // int e3 = (N+M) / 2; // The midpoint
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

	void tpsc();
	// if ( ae2 == ae3 || ae3 == ae4 ) {
	if ( compareXY(ae2, ae3) == 0 || compareXY(ae3, ae4) == 0 ) {
	  // Give up, cannot find good pivots
	  dflgm(A, N, M, e3, tpsc, depthLimit, compareXY);
	  return;
	}
	// Fix end points
	iswap(N, e2, A); iswap(M, e4, A);

	pl = A[N]; pr = A[M]; // they are there temporarily

	// initialize running indices
	i = N+1; j = M-1; 
	lw = e3-1; up = e3+1;

	// while ( A[i] < pl ) i++;
	while ( compareXY(A[i], pl) < 0 ) i++;
	// while ( pr < A[j] ) j--;
	while ( compareXY(pr, A[j]) < 0 ) j--;

 } else { // small <= L
     i = N; j = M;
     int middlex = N + (L>>1); // N + L/2

    int k, N1, M1; // for sampling
    int probeLng = sqrt(L/7); 
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    int third = probeLng/3;
    N1 = middlex - halfSegmentLng; //  N + (L>>1) - halfSegmentLng;
    M1 = N1 + probeLng - 1;
    int offset = L/probeLng;  

    // assemble the mini array [N1, M1]
    for (k = 0; k < probeLng; k++) // iswap(N1 + k, N + k * offset, A);
    { int xx = N1 + k, yy = N + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    if ( probeLng < 120 ) quicksort0c(A, N1, M1, depthLimit, compareXY); else {
      // protect against constant arrays
      int p0 = N1 + (probeLng>>1);
      int pn = N1, pm = M1, d = (probeLng-3)>>3;
      pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
      p0 = med(A, p0 - d, p0, p0 + d, compareXY);
      pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
      p0 = med(A, pn, p0, pm, compareXY);
      dflgm(A, N1, M1, p0, quicksort0c, depthLimit, compareXY);
    }

    lw = N1+third; up = M1-third;
    pl = A[lw]; pr = A[up];
    if ( compareXY(pl, A[middlex]) == 0 || 
	 compareXY( A[middlex], pr) == 0 ) {
	  // Give up, cannot find good pivots
	  dflgm(A, N, M, middlex, tpsc, depthLimit, compareXY);
	  return;
    }
    // Swap these two segments to the corners
    for ( k = N1; k <= lw-1; k++ ) {
      iswap(k, i, A); i++;
    }
    // i--;
    for ( k = M1; up+1 <= k; k--) {
      iswap(k, j, A); j--;
    }
    // j++;
    iswap(N, lw, A); iswap(M, up, A); // they are there temporarily 
  } 
        /* 
	  |)----------(--)-------------(|
	 N i         lw  up            j M
	  N < x < i -> A[x] < pl
	  lw < x < lup -> pl <= A[x] <= pr
	  j < x < M -> pr < A[x]
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
	  goto AIM;
	}
	// left gap closed
	i--;
	goto leftClosed;

 AIM:
	/* 
	  |)----------(--)-------------(|
	 N i         lw  up            j M
	 ai -> M
	*/
	if ( lw < i ) { i--; goto leftClosed; }
	// i <= lw
	am = A[lw];
	// if ( am < pl ) { // am -> L
	if ( compareXY(am, pl) < 0 ) {
	  A[i++] = am; A[lw--] = ai; 
	  if ( lw < i ) { i--; goto leftClosed; }
	  goto again;
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
	    goto again;
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
	  goto AIM;
	}
	// am -> M
	if ( i == lw ) { i--; goto leftClosed; }
	lw--; 
	goto AIM; 

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
	// iswap(N, i--, A); iswap(M, j++, A); // put the pivots in place
	iswap(N, i, A); i--; iswap(M, j, A); j++;
    /*
      |---]---------[---------|
      N   i         j         M
    */
	// tail iteration
      if ( i-N < j-i ) {
	tpsc(A, N, i, depthLimit, compareXY);
	if ( j-i < M-j ) {
	  tpsc(A, i+1, j-1, depthLimit, compareXY);
	   N = j; goto Start;
	}
	tpsc(A, j, M, depthLimit, compareXY);
	N = i+1; M = j-1; goto Start;
      }
      tpsc(A, i+1, j-1, depthLimit, compareXY);
      if ( i-N < M-j ) {
	tpsc(A, N, i, depthLimit, compareXY);
	N = j; goto Start;
      }
      tpsc(A, j, M, depthLimit, compareXY);
      M = i; goto Start;
} // end tpsc


