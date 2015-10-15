#include "mex.h"

/** Objectness headers
 */
#include "Objectness/Src/kyheader.h"
#include "Objectness/Src/Objectness.h"
#include "Objectness/Src/CmShow.h"
#include "Objectness/Src/DataSetVOC.h"
#include "Objectness/Src/ValStructVec.h"

/** C++
 */
#include <iostream>
using namespace std;

/** From mexOpencv
 */
#include "MxArray.hpp"

/** Input Arguments
 */
#define VOC_IN        prhs[0]
#define BASE_IN       prhs[1]
#define W_IN          prhs[2]
#define NSS_IN        prhs[3]
#define NUM_PER_SZ_IN prhs[4]

/** Output Arguments
 */
#define BB_OUT plhs[0]

/** Default Parameters
 */
#define _BASE 2
#define _W    8
#define _NSS  2
#define _NUM_PER_SZ 130

//extern "C" mxArray* mxCreateReference(mxArray*);

void mexFunction( int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[] )
{
  char*   voc       = NULL;

  double base = _BASE;
  int    W    = _W;
  int    NSS  = _NSS;
  int    numPerSz = _NUM_PER_SZ;

  if( nrhs < 1 ) mexErrMsgTxt("... [ At least 1 inputs required ( voc2007 )");
  if( nrhs > 5 ) mexErrMsgTxt("--- [ No more than 5 inputs required ( voc2007, base, W, NSS, numPerSz )");

  voc = mxArrayToString(VOC_IN);

  if( nrhs >= 2 )
  {
    base      = (double) mxGetScalar( BASE_IN );
  }
  if( nrhs >= 3 )
  {
    W         = (int)    mxGetScalar( W_IN );
  }
  if( nrhs >= 4 )
  {
    NSS       = (int)    mxGetScalar( NSS_IN );
  }
  if( nrhs == 5 )
  {
    numPerSz  = (int)    mxGetScalar( NUM_PER_SZ_IN );
  }

  const string str(voc);
  DataSetVOC voc2007( str );
  voc2007.loadAnnotations();

  printf("Dataset:`%s' with %d training and %d testing\n", _S(voc2007.wkDir), voc2007.trainNum, voc2007.testNum);
  printf("%s Base = %g, W = %d, NSS = %d, perSz = %d\n", _S(str), base, W, NSS, numPerSz);

  Objectness objectness( voc2007 );

  vector<vector<Vec4i>> _boxesTests;
  objectness.getObjBndBoxesForTestsFast( _boxesTests, numPerSz );

  int nTest = _boxesTests.size();
  BB_OUT = mxCreateCellMatrix(nTest, 1);

  int k, ix, iy, idx;
  for(k = 0; k < nTest; k++)
  {
    vector<Vec4i> bbs = _boxesTests[k];
    int nBB = bbs.size();
    mxArray* boxes = mxCreateNumericMatrix( nBB, 4, mxUINT32_CLASS, mxREAL );
    uint* outptr = (uint*) mxGetData(boxes);

    for(int iy = 0; iy < nBB; iy++ )
    {
      Vec4i bb = bbs[iy];
      for( ix = 0; ix < 4; ix++ )
      {
        idx = iy + ix * nBB;
        outptr[idx] = bb[ix];
      }
    }

    //mxSetCell(BB_OUT, k, mxCreateReference(boxes));
    //mxDestroyArray(boxes);
    mxSetCell(BB_OUT, k, boxes);
  }

  /** Clean memory
   */
  mxFree( voc );
}
