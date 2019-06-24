/*
 * Common.h
 *
 *  Created on: Apr 24, 2019
 *      Author: joseph
 */

#ifndef BC_BLAS_COMMON_H_
#define BC_BLAS_COMMON_H_

#include "../Array_Scalar_Constant.h"

namespace BC {
namespace exprs {

template<class,class,class>
struct Binary_Expression;

namespace blas_tools {

template<class derived>
struct Common_Tools {

	template<class Scalar_t, int alpha_mod, bool lv_scalar, bool rv_scalar, class Stream,  class lv_scalar_t, class rv_scalar_t>
	static auto calculate_alpha(Stream stream, lv_scalar_t lv, rv_scalar_t rv) {

		static constexpr bool lv_host_mode = (BC::exprs::expression_traits<lv_scalar_t>::is_stack_allocated);
		static constexpr bool rv_host_mode = (BC::exprs::expression_traits<rv_scalar_t>::is_stack_allocated);
		static_assert(lv_host_mode == rv_host_mode || lv_scalar != rv_scalar,
				"Host and Device Scalars may not be mixed Blas calculations");
		static constexpr bool host_mode    = lv_host_mode || rv_host_mode;

		if (host_mode || (!lv_scalar && !rv_scalar)) {
			stream.set_blas_pointer_mode_host();
		} else {
			stream.set_blas_pointer_mode_device();
		}

		return BC::meta::constexpr_if<!lv_scalar && !rv_scalar>(
					[&](){
						return make_constexpr_scalar<BC::host_tag, alpha_mod, Scalar_t>();
					},
				BC::meta::constexpr_else_if<(lv_scalar != rv_scalar) && (alpha_mod == 1)>(
					[&](){
						return BC::meta::constexpr_ternary<lv_scalar>(
								[&]() { return lv; },
								[&]() { return rv; }
						);
					},
				BC::meta::constexpr_else_if<lv_scalar && rv_scalar>(
					[&]() {
						return BC::meta::constexpr_ternary<host_mode>(
								[&](){
									return make_scalar_constant<BC::host_tag, Scalar_t>(alpha_mod * lv[0] * rv[0]);
								},[&](){
									auto tmp_scalar =  make_temporary_kernel_scalar<Scalar_t>(stream);
									derived::scalar_multiply(stream, tmp_scalar, alpha_mod, lv, rv);
									return tmp_scalar;
								});
					},
				BC::meta::constexpr_else_if<lv_scalar>(
						[&]() {
							return BC::meta::constexpr_if<host_mode>(
									[&](){
										return make_scalar_constant<BC::host_tag, Scalar_t>(alpha_mod * lv[0]);
									},[&](){
										auto tmp_scalar =  make_temporary_kernel_scalar<Scalar_t>(stream);
										derived::scalar_multiply(stream, tmp_scalar, alpha_mod, lv);
										return tmp_scalar;
									});
						}, [&]() { //else if rv_scalar
							return BC::meta::constexpr_if<host_mode>(
									[&](){
										return make_scalar_constant<BC::host_tag, Scalar_t>(alpha_mod * rv[0]);
									},[&](){
										auto tmp_scalar =  make_temporary_kernel_scalar<Scalar_t>(stream);
										derived::scalar_multiply(stream, tmp_scalar, alpha_mod, rv);
										return tmp_scalar;
									});
						})
				)));
	}

	template<
			class Lv,
			class Rv,
			class Alpha,
			class Beta,
			bool LvTrans,
			bool RvTrans,
			bool LvScalar,
			bool RvScalar>
	 struct contents {
        static constexpr bool lv_is_transposed = LvTrans;
        static constexpr bool rv_is_transposed = RvTrans;
        static constexpr bool lv_is_scalar_multiplied = LvScalar;
        static constexpr bool rv_is_scalar_multiplied = RvScalar;

		Lv left;
		Rv right;
		Alpha alpha;
		Beta beta;
	};

	template<int alpha_mod, int beta_mod, class Stream, class Lv, class Rv>
	static auto parse_expression(Stream stream, Lv left, Rv right) {
		/*
		 *	Strips transposition and scalar-multiplied from and left and right,
		 *	returns a 'contents' object. --
		 *
		 *	If left/right is/are transposed, calling 'dimensions(0), rows(), cols()'
		 *	will return the non-transposed dimensions/rows/cols. Ergo- you should use the original parameters
		 *	to access the shape of the returned value.
		 */

	    static constexpr bool lv_scalar = blas_expression_traits<Lv>::is_scalar_multiplied;
	    static constexpr bool rv_scalar = blas_expression_traits<Rv>::is_scalar_multiplied;
	    using value_type = typename Lv::value_type;

	    auto alpha_lv = blas_expression_traits<Lv>::get_scalar(left);
		auto alpha_rv = blas_expression_traits<Rv>::get_scalar(right);

		auto alpha_ = calculate_alpha<value_type, alpha_mod, lv_scalar, rv_scalar>(stream, alpha_lv, alpha_rv);
	    auto beta_  = make_constexpr_scalar<typename expression_traits<decltype(alpha_)>::allocation_tag, beta_mod, value_type>();//blas_impl::template scalar_constant<value_type, beta_mod>();

		auto left_ = greedy_evaluate(blas_expression_traits<Lv>::remove_blas_modifiers(left), stream);
	    auto right_ = greedy_evaluate(blas_expression_traits<Rv>::remove_blas_modifiers(right), stream);

	    using left_t = std::decay_t<decltype(left_)>;
	    using right_t = std::decay_t<decltype(right_)>;
	    using alpha_t = std::decay_t<decltype(alpha_)>;
	    using beta_t = std::decay_t<decltype(beta_)>;

	    return contents<
	    		left_t,
	    		right_t,
	    		alpha_t,
	    		beta_t,
	    	    blas_expression_traits<Lv>::is_transposed,
	    	    blas_expression_traits<Rv>::is_transposed,
	    	    blas_expression_traits<Lv>::is_scalar_multiplied,
	    	    blas_expression_traits<Rv>::is_scalar_multiplied> { left_, right_, alpha_, beta_ };
	}

	template<class Stream, class Contents>
	static void post_parse_expression_evaluation(Stream stream, Contents contents) {
		using value_type = typename decltype(contents.alpha)::value_type;
        BC::meta::constexpr_if<(BC::exprs::expression_traits<decltype(contents.alpha)>::is_temporary)>(
            BC::meta::bind([&](auto alpha) {
        		stream.template get_allocator_rebound<value_type>().deallocate(alpha, 1);
        	}, 	contents.alpha));

	}
};




}



}}

#endif
