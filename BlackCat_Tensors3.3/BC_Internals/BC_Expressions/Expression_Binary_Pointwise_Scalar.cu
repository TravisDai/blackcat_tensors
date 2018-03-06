
/*
 * BC_Expression_Binary_Pointwise_ScalarL.h
 *
 *  Created on: Dec 2, 2017
 *      Author: joseph
 */
#ifdef  __CUDACC__
#ifndef EXPRESSION_BINARY_POINTWISE_SCALAR_H_
#define EXPRESSION_BINARY_POINTWISE_SCALAR_H_

#include "Expression_Base.cu"
#include "BlackCat_Internal_Definitions.h"

namespace BC {

template<class T, class operation, class lv, class rv>
class binary_expression_scalar_L : expression<T,binary_expression_scalar_L<T, operation, lv, rv>> {
public:

	using this_type = binary_expression_scalar_L<T, operation, lv, rv>;

	operation oper;
	lv left;
	rv right;

	__BCinline__  int rank() const { return right.rank(); }
	__BCinline__  int rows() const { return right.rows(); };
	__BCinline__  int cols() const { return right.cols(); };
	__BCinline__  int size() const { return right.size(); };
	__BCinline__  int LD_rows() const { return right.LD_rows(); }
	__BCinline__  int LD_cols() const { return right.LD_cols(); }
	__BCinline__  int dimension(int i)		const { return right.dimension(i); }
	void printDimensions() 		const { right.printDimensions();   }
	void printLDDimensions()	const { right.printLDDimensions(); }
	__BCinline__ const auto innerShape() const 			{ return right.innerShape(); }
	__BCinline__ const auto outerShape() const 			{ return right().outerShape(); }

	__BCinline__ binary_expression_scalar_L(lv l, rv r) : left(l), right(r) {}
	__BCinline__  auto operator [](int index) const { return oper(left[0], right[index]); }
};

template<class T, class operation, class lv, class rv>
class binary_expression_scalar_R : expression<T, binary_expression_scalar_R<T, operation, lv, rv>> {
public:

	using this_type = binary_expression_scalar_R<T, operation, lv, rv>;

	operation oper;
	const lv left;
	const rv right;

	__BCinline__  int rank() const { return left.rank(); }
	__BCinline__  int rows() const { return left.rows(); };
	__BCinline__  int cols() const { return left.cols(); };
	__BCinline__  int size() const { return left.size(); };
	__BC_gcpu__ int LD_rows() const { return left.LD_rows(); }
	__BCinline__  int LD_cols() const { return left.LD_cols(); }
	__BCinline__  int dimension(int i)		const { return left.dimension(i); }
	void printDimensions() 		const { left.printDimensions();   }
	void printLDDimensions()	const { left.printLDDimensions(); }
	__BCinline__ const int* innerShape() const 			{ return left().innerShape(); }
	__BCinline__ const int* outerShape() const 			{ return left().outerShape(); }

	__BCinline__  binary_expression_scalar_R(lv l, rv r) : left(l), right(r) {}
	__BCinline__  auto operator [](int index) const { return oper(left[index], right[0]);}
};

template<class T, class operation, class lv, class rv>
class binary_expression_scalar_LR : expression<T, binary_expression_scalar_LR<T, operation, lv, rv>> {
public:
		lv left;
		rv right;
		operation oper;

		__BCinline__  int rank() const { return left.rank(); }
		__BCinline__  int rows() const { return left.rows(); };
		__BCinline__  int cols() const { return left.cols(); };
		__BCinline__  int size() const { return left.size(); };
		__BCinline__  int LD_rows() const { return left.LD_rows(); }
		__BCinline__  int LD_cols() const { return left.LD_cols(); }
		__BCinline__  int dimension(int i)		const { return left.dimension(i); }
		void printDimensions() 		const { left.printDimensions();   }
		void printLDDimensions()	const { left.printLDDimensions(); }
		__BCinline__  const auto innerShape() const 			{ return left().innerShape(); }
		__BCinline__  const auto outerShape() const 			{ return left().outerShape(); }

		__BCinline__  binary_expression_scalar_LR(lv l, rv r) : left(l), right(r) {}
		__BCinline__  auto operator [](int index) const { return oper(left[0], right[0]); }
};
}

#endif /* EXPRESSION_BINARY_POINTWISE_SCALAR_H_ */
#endif
