/*
   ARPACK++ v1.0 8/1/1997
   c++ interface to ARPACK code.

   MODULE RCompGReg.cc.
   Example program that illustrates how to solve a complex
   generalized eigenvalue problem in regular mode using 
   the ARrcCompGenEig class.

   1) Problem description:

      In this example we try to solve A*x = B*x*lambda in regular
      mode, where A and B are derived from the finite element 
      discretization of the 1-dimensional convection-diffusion operator
                          (d^2u/dx^2) + rho*(du/dx)
      on the interval [0,1] with zero boundary conditions using
      piecewise linear elements. 


   2) Data structure used to represent matrix A:

      ARrcCompGenEig is a class thar requires the user to provide a
      way to perform the matrix-vector products w = OPv = inv(B)*A*v
      and w = B*v. In this example a class called ComplexGenProblemA
      was created with this purpose. ComplexGenProblemA contains a
      member function, MultOPv(v,w), that takes a vector v and returns
      the product OPv in w. It also contains an object, B, that stores
      matrix B data. The product Bv is performed by MultMv, a member
      function of B.

   3) The reverse communication interface:

      This example uses the reverse communication interface, which
      means that the desired eigenvalues cannot be obtained directly
      from an ARPACK++ class.
      Here, the overall process of finding eigenvalues by using the
      Arnoldi method is splitted into two parts. In the first, a
      sequence of calls to a function called TakeStep is combined
      with matrix-vector products in order to find an Arnoldi basis.
      In the second part, an ARPACK++ function like FindEigenvectors
      (or EigenValVectors) is used to extract eigenvalues and
      eigenvectors.

   4) Included header files:

      File             Contents
      -----------      -------------------------------------------
      cgenprba.h       The ComplexGenProblemA class definition.
      arrgcomp.h       The ARrcCompGenEig class definition.
      rcompgsl.h       The Solution function.
      arcomp.h         The "arcomplex" (complex) type definition.

   5) ARPACK Authors:

      Richard Lehoucq
      Kristyn Maschhoff
      Danny Sorensen
      Chao Yang
      Dept. of Computational & Applied Mathematics
      Rice University
      Houston, Texas
*/

#include "arcomp.h"
#include "cgenprba.h"
#include "rcompgsl.h"
#include "arrgcomp.h"


template<class T>
void Test(T type)
{

  // Defining a complex pencil with n = 100.

  ComplexGenProblemA<T> P(100); // n = 100.

  // Creating a complex eigenvalue problem and defining what we need:
  // the four eigenvectors with largest magnitude.

  ARrcCompGenEig<T> prob(P.A.ncols(), 4L);

  // Finding an Arnoldi basis.

  while (!prob.ArnoldiBasisFound()) {

    // Calling ARPACK FORTRAN code. Almost all work needed to
    // find an Arnoldi basis is performed by TakeStep.

    prob.TakeStep();

    if ((prob.GetIdo() == 1)||(prob.GetIdo() == -1)) {

      // Performing w <- OP*v.
      // In regular mode, this product must be performed
      // whenever GetIdo is equal to 1 or -1. GetVector supplies
      // a pointer to the input vector, v, and PutVector a pointer
      // to the output vector, w.

      P.MultOPv(prob.GetVector(), prob.PutVector());

    }
    else if (prob.GetIdo() == 2) {

      // Performing w <- B*v.

      P.B.MultMv(prob.GetVector(), prob.PutVector());

    }
  }

  // Finding eigenvalues and eigenvectors.

  prob.FindEigenvectors();

  // Printing solution.

  Solution(prob);

} // Test.


main()
{

  // Solving a single precision problem with n = 100.

#ifndef __SUNPRO_CC

  Test((single)0.0);

#endif

  // Solving a double precision problem with n = 100.

  Test((double)0.0);

} // main

