/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BC_EXPRESSION_TEMPLATES_FUNCTION_GEMM_H_
#define BC_EXPRESSION_TEMPLATES_FUNCTION_GEMM_H_

#include "Expression_Base.h"
#include "BLAS_Feature_Detector.h"
#include "Tree_Evaluator_Runner.h"


namespace BC {
namespace et {


template<class lv, class rv, class System_Tag>
struct Binary_Expression<lv, rv, oper::gemm<System_Tag>>
: Expression_Base<Binary_Expression<lv, rv, oper::gemm<System_Tag>>>,
  BLAS_Function {

    static_assert(std::is_same<scalar_of<lv>, scalar_of<rv>>::value,\
    		"MATRIX MULTIPLICATION ONLY AVAILABLE TO SAME TYPE TENSORS (FLOAT/DOUBLE)");

    using value_type  = typename lv::value_type;
    using allocator_t = typename lv::allocator_t;
    using system_tag = System_Tag;
    using impl_l  = typename blas::implementation<system_tag>;
    using utility_l   = utility::implementation<system_tag>;
    using function_t = oper::gemm<System_Tag>;

    static constexpr bool transA = blas_feature_detector<lv>::transposed;
    static constexpr bool transB = blas_feature_detector<rv>::transposed;
    static constexpr bool lv_scalar = blas_feature_detector<lv>::scalar;
    static constexpr bool rv_scalar = blas_feature_detector<rv>::scalar;
    static constexpr bool lv_eval = blas_feature_detector<lv>::evaluate;
    static constexpr bool rv_eval = blas_feature_detector<rv>::evaluate;

    static constexpr int DIMS 	   = rv::DIMS;
    static constexpr int ITERATOR  = 1;


    lv left;
    rv right;


     Binary_Expression(lv left, rv right)
     : left(left), right(right) {}

    BCINLINE
    const auto inner_shape() const {
    	return make_lambda_array<DIMS>([&](int i) {
    		return i == 0 ? left.rows() : i == 1 ? right.cols() : 1;
    	});
    }
    BCINLINE
    const auto block_shape() const {
    	return make_lambda_array<DIMS>([&](int i) {
    		return i == 0 ? left.rows() : i == 1 ? size() : 1;
    	});
    }

    BCINLINE BC::size_t  size() const { return left.rows() * right.cols(); }
    BCINLINE BC::size_t  rows() const { return left.rows();  }
    BCINLINE BC::size_t  cols() const { return right.cols(); }
    BCINLINE BC::size_t  dimension(int i) const { return inner_shape()[i]; }
    BCINLINE BC::size_t  block_dimension(int i) const { return block_shape()[i]; }

    BCINLINE BC::size_t  M() const { return left.rows();  }
    BCINLINE BC::size_t  N() const { return right.cols(); }
    BCINLINE BC::size_t  K() const { return left.cols();  }


    template<class core, BC::size_t  alpha_mod, BC::size_t  beta_mod, class Context>
    void eval(tree::injector<core, alpha_mod, beta_mod> injection_values, Context& alloc) const {

        //get the data of the injection --> injector simply stores the alpha/beta scalar modifiers
        auto& injection = injection_values.data();

        //evaluate the left and right branches (computes only if necessary)
        auto A = CacheEvaluator<Context>::evaluate(blas_feature_detector<lv>::get_array(left), alloc);
        auto B = CacheEvaluator<Context>::evaluate(blas_feature_detector<rv>::get_array(right), alloc);

        //get the left and right side scalar values
        auto alpha_lv = blas_feature_detector<lv>::get_scalar(left);
        auto alpha_rv = blas_feature_detector<rv>::get_scalar(right);

        //allocate the alpha and beta scalars,
        auto alpha = alloc.scalar_alpha((value_type)alpha_mod);
        auto beta = alloc.scalar_constant<value_type, beta_mod>();

        //compute the scalar values if need be
        if (lv_scalar)
        	impl_l::scalar_mul(alpha, alpha, alpha_lv, alloc);
        if (rv_scalar)
        	impl_l::scalar_mul(alpha, alpha, alpha_rv, alloc);

        //call matrix_mul
        impl_l::gemm(alloc, transA, transB,  M(), N(), K(),
        		alpha, A, A.leading_dimension(0),
        		B, B.leading_dimension(0),
        		beta, injection, injection.leading_dimension(0));


        //deallocate all the temporaries
        if (lv_eval) cc(A).deallocate();
        if (rv_eval) cc(B).deallocate();
//        utility_l::deallocate(alpha);
    }
};


}
}


#endif /* EXPRESSION_BINARY_DOTPRODUCT_CU_ */
