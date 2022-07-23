// File: c:/bsd/rigel/sort/C3psort.c
// Date: Fri Dec 29 19:53:38 2017
/* This file has the source of the algorithms that make up parallel FiveSort
   headed by fivesort
*/

static const int cut3PLimit = 1875;

/*
#include "Isort.c"
#include "Hsort.c"
#include "Qusort.c"
#include "Dsort.c"
*/

static void tpsc();
// tps is the header function for the three partition sorter tpsc
void tps(void **A, int lo, int hi, int (*compar)()) {
  int L = hi - lo;
  if ( L < cut3PLimit ) { 
    cut2(A, lo, hi, compar);
    return;
  }
  int depthLimit = 1 + 2.9 * floor(log(L));
  tpsc(A, lo, hi, depthLimit, compar);
} // end tps

void tpsc(void **A, int lo, int hi, int depthLimit, int (*compareXY)()) {  
  // int z; // for tracing
  register int i, j, up, lw; // indices
  register void *ai, *aj, *am; // array values
  void *pl, *pr; // pivots
  int L;
 // A 3d recursive call is avoided by jumping back to Start.  
 Start:
  L = hi - lo;
  // printf("tpsc lo %i hi % i dl %i\n", lo,hi,depthLimit);
  if ( L <= 0 ) return;
  if ( depthLimit <= 0 ) { // prevent quadradic explosion
    heapc(A, lo, hi, compareXY);
    return;
  }
  if ( L < cut3PLimit ) {
    cut2c(A, lo, hi, depthLimit, compareXY);
    return;
  }
  depthLimit--;

  // const int small = 4400;
  /*
  if ( L < small ) { // use 5 elements for sampling
        int sixth = (L + 1) / 6;
        int e1 = lo  + sixth;
        int e5 = hi - sixth;
	int e3 = lo + L/2; // The midpoint
        // int e3 = (lo+hi) / 2; // The midpoint
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
	  dflgm(A, lo, hi, e3, tpsc, depthLimit, compareXY);
	  return;
	}
	// Fix end points
	iswap(lo, e2, A); iswap(hi, e4, A);

	pl = A[lo]; pr = A[hi]; // they are there temporarily

	// initialize running indices
	i = lo+1; j = hi-1; 
	lw = e3-1; up = e3+1;

	// while ( A[i] < pl ) i++;
	while ( compareXY(A[i], pl) < 0 ) i++;
	// while ( pr < A[j] ) j--;
	while ( compareXY(pr, A[j]) < 0 ) j--;

 } else { // small <= L
  */
     i = lo; j = hi;
     int middlex = lo + (L>>1); // lo + L/2

    int k, lo1, hi1; // for sampling
    int probeLng = sqrt(L/7); 
    int halfSegmentLng = probeLng >> 1; // probeLng/2;
    int third = probeLng/3;
    lo1 = middlex - halfSegmentLng; //  lo + (L>>1) - halfSegmentLng;
    hi1 = lo1 + probeLng - 1;
    int offset = L/probeLng;  

    // assemble the mini array [lo1, hi1]
    for (k = 0; k < probeLng; k++) // iswap(lo1 + k, lo + k * offset, A);
    { int xx = lo1 + k, yy = lo + k * offset; iswap(xx, yy, A); }
    // sort this mini array to obtain good pivots
    /*
    if ( probeLng < 120 ) quicksort0c(A, lo1, hi1, depthLimit, compareXY); 
    else {
    // protect against constant arrays
    int p0 = lo1 + (probeLng>>1);
    int pn = lo1, pm = hi1, d = (probeLng-3)>>3;
    pn = med(A, pn, pn + d, pn + 2 * d, compareXY);
    p0 = med(A, p0 - d, p0, p0 + d, compareXY);
    pm = med(A, pm - 2 * d, pm - d, pm, compareXY);
    p0 = med(A, pn, p0, pm, compareXY);
    if ( p0 != middlex ) iswap(p0, middlex, A);
    dflgm(A, lo1, hi1, middlex, quicksort0c, depthLimit, compareXY);
    }
    */
    // quicksort0c(A, lo1, hi1, depthLimit, compareXY); 
    cut2c(A, lo1, hi1, depthLimit, compareXY); 
    lw = lo1+third; up = hi1-third;
    pl = A[lw]; pr = A[up];
    if ( compareXY(pl, pr) == 0 || 
	 compareXY(pr, A[hi1) == 0 ) {
	  // Give up, cannot find good pivots
	  dflgm(A, lo, hi, middlex, tpsc, depthLimit, compareXY);
	  return;
    }
    // Swap these two segments to the corners
    for ( k = lo1; k <= lw-1; k++ ) {
      iswap(k, i, A); i++;
    }
    // i--;
    for ( k = hi1; up+1 <= k; k--) {
      iswap(k, j, A); j--;
    }
    // j++;
    iswap(lo, lw, A); iswap(hi, up, A); // they are there temporarily 
 //  }

	/* 
	  |)----------(--)-------------(|
	lo i         lw  up            j hi
	  lo <= x < i -> A[x] < pl
	  lw < x < lup -> pl <= A[x] <= pr
	  j < x <= hi -> pr < A[x]
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
	    // aj -> hi
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
	lo i         lw  up            j hi
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
	lo i         lw  up            j hi
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
	lo i         lw  up            j hi
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
	lo i         lw  up            j hi
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
	lo   i            up            j hi
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
	  lo i         lw  j             hi
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
	  lo i         lw  j             hi
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
	  lo i         lw  j             hi
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
     lo   i         j         hi
    */
    // printf("tpsc done lo %i hi % i dl %i\n", lo,hi,depthLimit);
    /*
      for ( z = lo; z <= i; z++ )
	//	if ( pl <= A[z] ) {
	if ( compareXY(pl, A[z]) <= 0 ) {
	  printf("doneL z %i\n", z);
	  printf("lo %i i %i lw %i up %i j %i hi %i\n", lo,i,lw,up,j,hi);
	  exit(0);
	}
      for ( z = i+1; z < j; z++ )
	//	if ( A[z] < pl || pr < A[z] ) {
	if ( compareXY(A[z], pl) < 0 || pr < A[z] ) {
	  printf("doneM z %i\n", z);
	  printf("lo %i i %i lw %i up %i j %i hi %i\n", lo,i,lw,up,j,hi);
	  exit(0);
	}
      for ( z = j; z <= hi ; z++ )
	//	if ( A[z] <= pr ) {
	if ( compareXY(A[z], pr) <= 0 ) {
	  printf("doneR z %i\n", z);
	  printf("lo %i i %i lw %i up %i j %i hi %i\n", lo,i,lw,up,j,hi);
	  exit(0);
	}
    */
	// tpsc(A, lo, i, depthLimit, compareXY);
	// tpsc(A, i+1, j-1, depthLimit, compareXY);
	// tpsc(A, j, hi, depthLimit, compareXY);

	if ( i - lo < j - i ) {
	  if ( j - i < hi - j ) {
	    addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, i+1,j-1, depthLimit, compareXY));
	  } else {
	    addTaskSynchronized(ll, newTask(A, i+1,j-1, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	  }
	  hi = i;
	  goto Start;
	}
	if ( j - i < hi - j ) {
	  if ( i - lo < hi - j) {
	    addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, lo, i, depthLimit, compareXY));
	  } else {
	    addTaskSynchronized(ll, newTask(A, lo, i, depthLimit, compareXY));
	    addTaskSynchronized(ll, newTask(A, j, hi, depthLimit, compareXY));
	  }
	  lo = i+1;
	  hi = j-1;
	  goto Start;
	}
	addTaskSynchronized(ll, newTask(A, lo, i, depthLimit, compareXY));
	addTaskSynchronized(ll, newTask(A, i+1,j-1, depthLimit, compareXY));
	lo = j;
	goto Start;
} // end tpsc
