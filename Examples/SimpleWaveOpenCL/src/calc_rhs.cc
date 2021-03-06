/*  File produced by Kranc */

#define KRANC_C

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cctk.h"
#include "cctk_Arguments.h"
#include "cctk_Parameters.h"
#include "GenericFD.h"
#include "Differencing.h"
#include "cctk_Loop.h"
#include "loopcontrol.h"
#include "OpenCLRunTime.h"

/* Define macros used in calculations */
#define INITVALUE (42)
#define QAD(x) (SQR(SQR(x)))
#define INV(x) ((1.0) / (x))
#define SQR(x) ((x) * (x))
#define CUB(x) ((x) * (x) * (x))

extern "C" void calc_rhs_SelectBCs(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  CCTK_INT ierr = 0;
  ierr = Boundary_SelectGroupForBC(cctkGH, CCTK_ALL_FACES, GenericFD_GetBoundaryWidth(cctkGH), -1 /* no table */, "SimpleWaveOpenCL::evolved_grouprhs","flat");
  if (ierr < 0)
    CCTK_WARN(1, "Failed to register flat BC for SimpleWaveOpenCL::evolved_grouprhs.");
  return;
}

static void calc_rhs_Body(cGH const * restrict const cctkGH, int const dir, int const face, CCTK_REAL const normal[3], CCTK_REAL const tangentA[3], CCTK_REAL const tangentB[3], int const imin[3], int const imax[3], int const n_subblock_gfs, CCTK_REAL * restrict const subblock_gfs[])
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  char const * const source =
  "\n"
  "/* Include user-supplied include files */\n"
  "\n"
  "/* Initialise finite differencing variables */\n"
  "ptrdiff_t const di = 1;\n"
  "ptrdiff_t const dj = CCTK_GFINDEX3D(cctkGH,0,1,0) - CCTK_GFINDEX3D(cctkGH,0,0,0);\n"
  "ptrdiff_t const dk = CCTK_GFINDEX3D(cctkGH,0,0,1) - CCTK_GFINDEX3D(cctkGH,0,0,0);\n"
  "ptrdiff_t const cdi = sizeof(CCTK_REAL) * di;\n"
  "ptrdiff_t const cdj = sizeof(CCTK_REAL) * dj;\n"
  "ptrdiff_t const cdk = sizeof(CCTK_REAL) * dk;\n"
  "CCTK_REAL const dx = ToReal(CCTK_DELTA_SPACE(0));\n"
  "CCTK_REAL const dy = ToReal(CCTK_DELTA_SPACE(1));\n"
  "CCTK_REAL const dz = ToReal(CCTK_DELTA_SPACE(2));\n"
  "CCTK_REAL const dt = ToReal(CCTK_DELTA_TIME);\n"
  "CCTK_REAL const t = ToReal(cctk_time);\n"
  "CCTK_REAL const dxi = INV(dx);\n"
  "CCTK_REAL const dyi = INV(dy);\n"
  "CCTK_REAL const dzi = INV(dz);\n"
  "CCTK_REAL const khalf = 0.5;\n"
  "CCTK_REAL const kthird = 1/3.0;\n"
  "CCTK_REAL const ktwothird = 2.0/3.0;\n"
  "CCTK_REAL const kfourthird = 4.0/3.0;\n"
  "CCTK_REAL const keightthird = 8.0/3.0;\n"
  "CCTK_REAL const hdxi = 0.5 * dxi;\n"
  "CCTK_REAL const hdyi = 0.5 * dyi;\n"
  "CCTK_REAL const hdzi = 0.5 * dzi;\n"
  "\n"
  "/* Initialize predefined quantities */\n"
  "CCTK_REAL const p1o2dx = 0.5*INV(dx);\n"
  "CCTK_REAL const p1o2dy = 0.5*INV(dy);\n"
  "CCTK_REAL const p1o2dz = 0.5*INV(dz);\n"
  "CCTK_REAL const p1odx2 = INV(SQR(dx));\n"
  "CCTK_REAL const p1ody2 = INV(SQR(dy));\n"
  "CCTK_REAL const p1odz2 = INV(SQR(dz));\n"
  "\n"
  "/* Assign local copies of arrays functions */\n"
  "\n"
  "\n"
  "\n"
  "/* Calculate temporaries and arrays functions */\n"
  "\n"
  "/* Copy local copies back to grid functions */\n"
  "\n"
  "/* Loop over the grid points */\n"
  "#pragma omp parallel\n"
  "CCTK_LOOP3 (calc_rhs,\n"
  "  i,j,k, imin[0],imin[1],imin[2], imax[0],imax[1],imax[2],\n"
  "  cctk_lsh[0],cctk_lsh[1],cctk_lsh[2])\n"
  "{\n"
  "  ptrdiff_t const index = di*i + dj*j + dk*k;\n"
  "  \n"
  "  /* Assign local copies of grid functions */\n"
  "  \n"
  "  CCTK_REAL phiL = phi[index];\n"
  "  CCTK_REAL piL = pi[index];\n"
  "  \n"
  "  \n"
  "  /* Include user supplied include files */\n"
  "  \n"
  "  /* Precompute derivatives */\n"
  "  CCTK_REAL const PDstandard2nd11phi = PDstandard2nd11(&phi[index]);\n"
  "  CCTK_REAL const PDstandard2nd22phi = PDstandard2nd22(&phi[index]);\n"
  "  CCTK_REAL const PDstandard2nd33phi = PDstandard2nd33(&phi[index]);\n"
  "  \n"
  "  /* Calculate temporaries and grid functions */\n"
  "  CCTK_REAL phirhsL = piL;\n"
  "  \n"
  "  CCTK_REAL pirhsL = PDstandard2nd11phi + PDstandard2nd22phi + \n"
  "    PDstandard2nd33phi;\n"
  "  \n"
  "  /* Copy local copies back to grid functions */\n"
  "  vec_store_nta_partial(phirhs[index],phirhsL);\n"
  "  vec_store_nta_partial(pirhs[index],pirhsL);\n"
  "}\n"
  "CCTK_ENDLOOP3 (calc_rhs);\n"
  ""
  ;
  
  char const * const groups[] = {"SimpleWaveOpenCL::evolved_group","SimpleWaveOpenCL::evolved_grouprhs",NULL};
  
  static struct OpenCLKernel * kernel = NULL;
  char const * const sources[] = {differencing, source, NULL};
  OpenCLRunTime_CallKernel (cctkGH, CCTK_THORNSTRING, "calc_rhs",
                            sources, groups, NULL, NULL, NULL, -1,
                            imin, imax, &kernel);
  
}

extern "C" void calc_rhs(CCTK_ARGUMENTS)
{
  DECLARE_CCTK_ARGUMENTS;
  DECLARE_CCTK_PARAMETERS;
  
  
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Entering calc_rhs_Body");
  }
  
  if (cctk_iteration % calc_rhs_calc_every != calc_rhs_calc_offset)
  {
    return;
  }
  
  const char *groups[] = {"SimpleWaveOpenCL::evolved_group","SimpleWaveOpenCL::evolved_grouprhs"};
  GenericFD_AssertGroupStorage(cctkGH, "calc_rhs", 2, groups);
  
  GenericFD_EnsureStencilFits(cctkGH, "calc_rhs", 1, 1, 1);
  
  GenericFD_LoopOverInterior(cctkGH, &calc_rhs_Body);
  
  if (verbose > 1)
  {
    CCTK_VInfo(CCTK_THORNSTRING,"Leaving calc_rhs_Body");
  }
}
