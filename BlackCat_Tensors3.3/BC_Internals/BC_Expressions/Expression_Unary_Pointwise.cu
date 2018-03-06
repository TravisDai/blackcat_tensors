/*
 * Expression_Unary_Pointwise.cu
 *
 *  Created on: Jan 25, 2018
 *      Author: joseph
 */
#ifdef  __CUDACC__
#ifndef EXPRESSION_UNARY_POINTWISE_CU_
#define EXPRESSION_UNARY_POINTWISE_CU_

#include "Expression_Base.cu"
namespace BC {
template<class T, class operation, class value>
class unary_expression : public expression<T, unary_expression<T, operation, value>> {
public:

	operation oper;
	value array;

	__BCinline__  unary_expression(value v) : array(v) {}
	__BCinline__  int rank() const { return array.rank(); }
	__BCinline__  int size() const { return array.size(); }
	__BCinline__  int rows() const { return array.rows(); }
	__BCinline__  int cols() const { return array.cols(); }
	__BCinline__  int LD_rows() const { return array.LD_rows(); }
	__BCinline__  int LD_cols() const { return array.LD_cols(); }
	__BCinline__  int dimension(int i)		const { return array.dimension(i); }
	__BCinline__  const auto innerShape() const 			{ return array.innerShape(); }
	__BCinline__  const auto outerShape() const 			{ return array.outerShape(); }

	__BCinline__ auto operator [](int index) const -> decltype(oper(array[index])) {
		return oper(array[index]);
	}
	__BCinline__ auto operator [](int index) -> decltype(oper(array[index])) {
		return oper(array[index]);
	}

	void printDimensions() 		const { array.printDimensions();   }
	void printLDDimensions()	const { array.printLDDimensions(); }


};
}
#endif /* EXPRESSION_UNARY_POINTWISE_CU_ */
#endif
