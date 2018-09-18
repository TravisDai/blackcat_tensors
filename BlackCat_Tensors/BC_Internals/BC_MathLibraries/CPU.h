/*
 * BC_Mathematics_CPU.h
 *
 *  Created on: Dec 1, 2017
 *      Author: joseph
 */

#ifndef MATHEMATICS_CPU_H_
#define MATHEMATICS_CPU_H_

#include "Print.h"

#include "Math_Library_Directives.h"

#include "CPU_Implementation/CPU_Utility.h"
#include "CPU_Implementation/CPU_Misc.h"
#include "CPU_Implementation/CPU_BLAS.h"
#include "CPU_Implementation/CPU_BLAS.h"
#include "CPU_Implementation/CPU_Constants.h"
#include "CPU_Implementation/CPU_Convolution.h"



namespace BC {

/*
 * The core CPU library,
 *
 * defines:
 * 	copy(T, U, int)
 *
 * 	dimensional evaluations
 *
 */

class CPU:
		public CPU_Utility<CPU>,
		public CPU_Misc<CPU>,
		public CPU_BLAS<CPU>,
		public CPU_Constants<CPU>//,
//		public CPU_Convolution<CPU>

{


public:

	static constexpr int SINGLE_THREAD_THRESHOLD = 8192;

//-------------------------------Generic Copy ---------------------------------//
	template<typename T, typename J>
	static void copy(T& t, const J& j, int sz) {
 __BC_omp_for__
		for (int i = 0; i < sz; ++i) {
			t[i] = j[i];
		}
 __BC_omp_bar__
	}
	//-------------------------------1d eval/copy ---------------------------------//

	struct n0 {
		template<class T>
		static void eval(T to) {
 __BC_omp_for__
			for (int i = 0; i < to.size(); ++i) {
				to[i];
			}
		}
	};

	//-------------------------------2d eval/copy ---------------------------------//
	struct n2 {
		template<class T>
		static void eval(T to) {
 __BC_omp_for__
			for (int n = 0; n < to.dimension(1); ++n)
				for (int m = 0; m < to.dimension(0); ++m)
					to(m, n);
		}
	};
	//-------------------------------3d eval/copy ---------------------------------//

	struct n3 {
		template<class T>
		static void eval(T to) {

 __BC_omp_for__

			for (int k = 0; k < to.dimension(2); ++k)
				for (int n = 0; n < to.dimension(1); ++n)
					for (int m = 0; m < to.dimension(0); ++m)
						to(m, n, k);
		}
	};
	//-------------------------------4d eval/copy ---------------------------------//4
	struct n4 {
		template<class T>
		static void eval(T to) {

 __BC_omp_for__
			for (int p = 0; p < to.dimension(3); ++p)
				for (int k = 0; k < to.dimension(2); ++k)
					for (int n = 0; n < to.dimension(1); ++n)
						for (int m = 0; m < to.dimension(0); ++m)
							to(m, n, k, p);
		}
	};
	//-------------------------------5d eval/copy ---------------------------------//

	struct n5 {
		template<class T>
		static void eval(T to) {

 __BC_omp_for__
			for (int j = 0; j < to.dimension(4); ++j)
				for (int p = 0; p < to.dimension(3); ++p)
					for (int k = 0; k < to.dimension(2); ++k)
						for (int n = 0; n < to.dimension(1); ++n)
							for (int m = 0; m < to.dimension(0); ++m)
								to(m, n, k, p, j);
		}
	};
	//-------------------------------implementation ---------------------------------//

	template<int d>
	struct dimension {
		using run = std::conditional_t<(d <= 1), n0,
		std::conditional_t< d ==2, n2,
		std::conditional_t< d == 3, n3,
		std::conditional_t< d == 4, n4, n5>>>>;

		template<class T>
		static void eval(T to) {
			run::eval(to);
#ifndef __BC_parallel_section__
 __BC_omp_bar__
#endif
		}
	};
};
}
#endif
 /* MATHEMATICS_CPU_H_ */