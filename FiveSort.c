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

// File: c:/bsd/rigel/sort/FiveSort.c
// Date: Sat Dec 28 15:30:11 2013
/* This file has the source of the algorithms that make up FiveSort
   headed by fivesort
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
#include <math.h>

// const int cut2Limit = 127;
extern int cut2Limit;
const int cut3Limit = 250;

char* expiration = "*** License for fivesort has expired ...\n";

// Here more global entities used throughout
int (*compareXY)();
void **A;

#include "Isort"
#include "Hsort"
#include "Qusort"
#include "Dsort"
// #include "C2sort"  // used by UseFiveSort


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
} // end tpsc


void fivesort(void **AA, int size, 
	int (*compar ) (const void *, const void * ) ) {
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
  tps(0, size-1);
}
/* compile with: 
   gcc -c FiveSort.c
   which produces:
   FiveSort.o
   Compile & link with UseFiveSort:
   gcc UseFiveSort.c FiveSort.o
*/
