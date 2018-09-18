/*
 * Math_Library_Directives.h
 *
 *  Created on: Aug 1, 2018
 *      Author: joseph
 */

#ifndef MATH_LIBRARY_DIRECTIVES_H_
#define MATH_LIBRARY_DIRECTIVES_H_


#ifdef _OPENMP
#ifndef BC_NO_OPENMP
#define __BC_omp_for__ _Pragma("omp parallel for")
#define __BC_omp_bar__ _Pragma("omp barrier")
#endif
#else
#define __BC_omp_for__
#define __BC_omp_bar__
#endif

#define pragma_BC_parallel #define __BC_PARALLEL_SECTION__
#define pragma_bc_barrier  #undef  __BC_PARALLEL_SECTION__

#endif /* MATH_LIBRARY_DIRECTIVES_H_ */
