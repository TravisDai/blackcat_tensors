/*  Project: BlackCat_Tensors
 *  Author: Joseph Jaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef PTE_ARRAY_H_
#define PTE_ARRAY_H_

#ifdef BC_TREE_OPTIMIZER_DEBUG
#define BC_TREE_OPTIMIZER_STDOUT(literal) std::cout << literal << std::endl;
#else
#define BC_TREE_OPTIMIZER_STDOUT(literal)
#endif

#include <type_traits>
#include "Common.h"
#include "Tree_Struct_Injector.h"
#include "Tree_Functions.h"
#include "Array.h"
#include "Expression_Binary.h"
#include "Expression_Unary.h"


namespace BC {
namespace et {
namespace tree {


template<class T>
struct evaluator_default {
	/*
	 * entirely_blas_expr -- if we may replace this branch entirely with a temporary/cache
	 * partial_blas_expr  -- if part of this branch contains a replaceable branch nested inside it
	 * nested_blas_expr   -- if a replaceable branch is inside a function (+=/-= won't work but basic assign = can work)
	 */

    static constexpr bool entirely_blas_expr = false;			//An expression of all +/- operands and BLAS calls				IE w*x + y*z
    static constexpr bool partial_blas_expr = false;			//An expression of element-wise +/- operations and BLAS calls	IE w + x*y
    static constexpr bool nested_blas_expr  = false;			//An expression containing a BLAS expression nested in a unary_functor IE abs(w * x)
    static constexpr bool requires_greedy_eval = false;			//Basic check if any BLAS call exists at all

    template<class core, int a, int b, class Allocator>
    static auto linear_evaluation(const T& branch, injector<core, a, b> tensor, Allocator&) {
        return branch;
    }

    template<class core, int a, int b, class Allocator>
    static auto injection(const T& branch, injector<core, a, b> tensor, Allocator&) {
        return branch;
    }

    template<class Allocator>
    static auto temporary_injection(const T& branch, Allocator& alloc) {
        return branch;
    }

    template<class Allocator>
    static void deallocate_temporaries(const T, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("DEFAULT DEALLOCATE TEMPORARIES");
        return;
    }
};



template<class T, class voider=void>
struct evaluator;

//-------------------------------- Array ----------------------------------------------------//
template<class T>
struct evaluator<T, std::enable_if_t<is_array<T>() && !is_temporary<T>()>>
: evaluator_default<T> {};

//--------------Temporary---------------------------------------------------------------------//

template<int x, class Scalar, class Allocator>
struct evaluator<
	ArrayExpression<x, Scalar, Allocator, BC_Temporary>,
	std::enable_if_t<BC::is_temporary<ArrayExpression<x, Scalar, Allocator, BC_Temporary>>()>>
 : evaluator_default<ArrayExpression<x, Scalar, Allocator, BC_Temporary>> {


	template<class Allocator_>
    static void deallocate_temporaries(ArrayExpression<x, Scalar, Allocator, BC_Temporary> tmp, Allocator_& alloc) {
        alloc.deallocate(tmp.array, tmp.size());
    }
};


//-----------------------------------------------BLAS----------------------------------------//



template<class lv, class rv, class op>
struct evaluator<Binary_Expression<lv, rv, op>, std::enable_if_t<is_blas_func<op>()>> {
    static constexpr bool entirely_blas_expr = true;
    static constexpr bool partial_blas_expr = true;
    static constexpr bool nested_blas_expr = true;
    static constexpr bool requires_greedy_eval = true;


    using branch_t = Binary_Expression<lv, rv, op>;

    template<class core, int a, int b, class Allocator>
    static auto linear_evaluation(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("BLAS_EXPR: linear_evaluation" << "alpha=" << a << "beta=" << b);

    	branch.eval(tensor, alloc);
        return tensor.data();
    }
    template<class core, int a, int b, class Allocator>
    static auto injection(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("BLAS_EXPR: injection");
        branch.eval(tensor, alloc);
        return tensor.data();
    }

    //if no replacement is used yet, auto inject
    template<class Allocator>
    static auto temporary_injection(const Binary_Expression<lv, rv, op>& branch, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("BLAS_EXPR: temporary_injection");

    	using param = Binary_Expression<lv, rv, op>;
    	using value_type = typename Allocator::value_type;
    	constexpr int dims = param::DIMS;

    	using tmp_t = ArrayExpression<dims, value_type, Allocator, BC_Temporary>;
        tmp_t tmp;
        tmp.as_shape() = Shape<dims>(branch.inner_shape());
        tmp.array = alloc.allocate(tmp.size());

        //ISSUE HERE
        branch.eval(make_injection<1, 0>(tmp.internal()), alloc);
        return tmp;
    }

    template<class Allocator>
    static void deallocate_temporaries(const Binary_Expression<lv, rv, op>& branch, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("BLAS_EXPR: deallocate_temporaries");

        evaluator<lv>::deallocate_temporaries(branch.left, alloc);
        evaluator<rv>::deallocate_temporaries(branch.right, alloc);
    }
};


//-------------------------------- Linear ----------------------------------------------------//


template<class lv, class rv, class op>
struct evaluator<Binary_Expression<lv, rv, op>, std::enable_if_t<is_linear_op<op>()>> {
    static constexpr bool entirely_blas_expr 	= evaluator<lv>::entirely_blas_expr && evaluator<rv>::entirely_blas_expr;
    static constexpr bool partial_blas_expr 	= evaluator<lv>::partial_blas_expr || evaluator<rv>::partial_blas_expr;
    static constexpr bool nested_blas_expr 		= evaluator<lv>::nested_blas_expr || evaluator<rv>::nested_blas_expr;
    static constexpr bool requires_greedy_eval 	= evaluator<lv>::requires_greedy_eval || evaluator<rv>::requires_greedy_eval;


    //-------------Linear evaluation branches---------------------//


    //needed by injection and linear_evaluation
    struct remove_branch {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- remove_branch (entire)");

        	evaluator<lv>::linear_evaluation(branch.left, tensor);
        	evaluator<rv>::linear_evaluation(branch.right, update_injection<op, true>(tensor, alloc));
        	return tensor.data();
        }
    };

    //if left is entirely blas_expr
    struct remove_left_branch {

        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- remove_left_branch");

        	/*auto left = */ evaluator<lv>::linear_evaluation(branch.left, tensor, alloc);
            auto right = evaluator<rv>::linear_evaluation(branch.right, update_injection<op, true>(tensor), alloc);
            return right;
        }
    };
    struct remove_left_branch_and_negate {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, oper::sub>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- remove_left_branch");

        	/*auto left = */ evaluator<lv>::linear_evaluation(branch.left, tensor, alloc);
            auto right = evaluator<rv>::linear_evaluation(branch.right, update_injection<op, true>(tensor), alloc);
            return make_un_expr<oper::negation>(right);
        }
    };

    //if right is entirely blas_expr (or if no blas expr)

    struct remove_right_branch {

    	template<class core, int a, int b, class Allocator>
    	static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- remove_right_branch");

    		auto left  = evaluator<lv>::linear_evaluation(branch.left, tensor, alloc);
          /*auto right = */ evaluator<rv>::linear_evaluation(branch.right, update_injection<op, b != 0>(tensor), alloc);
            return left;
        }
    };

    struct basic_eval {

        template<class core, int a, int b, class Allocator>
    	static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- basic_eval");

        	static constexpr bool left_evaluated = evaluator<lv>::partial_blas_expr || b != 0;
        	auto left = evaluator<lv>::linear_evaluation(branch.left, tensor, alloc);
            auto right = evaluator<rv>::linear_evaluation(branch.right, update_injection<op, left_evaluated>(tensor), alloc);
            return make_bin_expr<op>(left, right);
        }
    };


    template<class core, int a, int b, class Allocator>
    static auto linear_evaluation(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("Binary Linear: linear_evaluation");

        using impl =
        		std::conditional_t<entirely_blas_expr, remove_branch,
        		std::conditional_t<evaluator<lv>::entirely_blas_expr,
        		std::conditional_t<std::is_same<op, oper::sub>::value, remove_left_branch_and_negate, remove_left_branch>,
        		std::conditional_t<evaluator<rv>::entirely_blas_expr, remove_right_branch,
        		basic_eval>>>;

        return impl::function(branch, tensor, alloc);
    }

    //---------------partial blas expr branches-------------------------//
    struct evaluate_branch {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- evaluate_branch");
        	auto left = evaluator<lv>::linear_evaluation(branch.left, tensor);
        	auto right = evaluator<rv>::linear_evaluation(branch.right, update_injection<op, true>(tensor));
            return make_bin_expr<op>(left, right);
        }
    };


    struct left_blas_expr {

        struct trivial_injection {
            template<class l, class r, class Allocator>
            static auto function(const l& left, const r& right, Allocator&) {
	        	BC_TREE_OPTIMIZER_STDOUT("-- trivial_injection");
            	return left;
            }
        };

		struct non_trivial_injection {
			template<class LeftVal, class RightVal, class Allocator>
			static auto function(const LeftVal& left, const RightVal& right, Allocator&) {
	        	BC_TREE_OPTIMIZER_STDOUT("-- non_trivial_injection");
	            return make_bin_expr<op>(left, right);
			}
		};

        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- left_blas_expr");

            auto left = evaluator<lv>::injection(branch.left, tensor, alloc);

            //check this ?
            auto right = evaluator<rv>::linear_evaluation(branch.right, update_injection<op, true>(tensor), alloc);

            using impl = std::conditional_t<evaluator<rv>::entirely_blas_expr,
                    trivial_injection, non_trivial_injection>;

            return impl::function(left, right, alloc);
        }
    };
    struct right_blas_expr {
        struct trivial_injection {
            template<class l, class r, class Allocator>
            static auto function(const l& left, const r& right, Allocator& alloc) {
            	BC_TREE_OPTIMIZER_STDOUT("-- trivial_injection");
                return right;
            }
        };
        struct non_trivial_injection {
                template<class l, class r, class Allocator>
                static auto function(const l& left, const r& right, Allocator& alloc) {
                	BC_TREE_OPTIMIZER_STDOUT("-- non_trivial_injection");
                    return make_bin_expr<op>(left, right);
                }
            };
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- right_blas_expr");

            auto left = evaluator<lv>::linear_evaluation(branch.left, tensor, alloc);
            auto right = evaluator<rv>::injection(branch.right, tensor, alloc);

            using impl = std::conditional_t<evaluator<lv>::entirely_blas_expr,
                    trivial_injection, non_trivial_injection>;

            return impl::function(left, right, alloc);
        }
    };

    //------------nontrivial injections, DO NOT UPDATE, scalars-mods will enact during elementwise-evaluator----------------//
    struct left_nested_blas_expr {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- left_nested_blas_expr");

            auto left = evaluator<lv>::injection(branch.left, tensor, alloc);
            rv right = branch.right; //rv
            return make_bin_expr<op>(left, right);
        }
    };
    struct right_nested_blas_expr {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        	BC_TREE_OPTIMIZER_STDOUT("- right_nested_blas_expr");

            lv left = branch.left; //lv
            auto right = evaluator<rv>::injection(branch.right, tensor, alloc);
            return make_bin_expr<op>(left, right);
        }
    };


    template<class core, int a, int b, class Allocator>
    static auto injection(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("Binary Linear: Injection");

    	struct to_linear_eval {
    		static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
    	    	BC_TREE_OPTIMIZER_STDOUT("-flip to linear_eval");

    			return linear_evaluation(branch, tensor, alloc);
    		}
    	};

        using impl =
          		std::conditional_t<entirely_blas_expr, remove_branch,
        		std::conditional_t<evaluator<rv>::partial_blas_expr && evaluator<lv>::partial_blas_expr, basic_eval,
        		std::conditional_t<evaluator<lv>::nested_blas_expr, left_nested_blas_expr,
                std::conditional_t<evaluator<rv>::nested_blas_expr, right_nested_blas_expr, basic_eval>>>>;

        static_assert(!std::is_same<void, impl>::value, "EXPRESSION_REORDERING COMPILATION FAILURE, USE 'ALIAS' AS A WORKAROUND");
        return impl::function(branch, tensor, alloc);
    }


    //---------substitution implementation---------//

    template<class allocator>
    static auto temporary_injection(const Binary_Expression<lv,rv,op>& branch, allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("Binary Linear: Temporary Injection");

    	auto left  = evaluator<lv>::template temporary_injection<allocator>(branch.left, alloc);
    	auto right = evaluator<rv>::template temporary_injection<allocator>(branch.right, alloc);
    	return make_bin_expr<op>(left, right);
    }


    template<class Allocator>
    static void deallocate_temporaries(const Binary_Expression<lv, rv, op>& branch, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("Binary Linear: Deallocate Temporaries");

        evaluator<lv>::deallocate_temporaries(branch.left, alloc);
        evaluator<rv>::deallocate_temporaries(branch.right, alloc);
    }

};

//------------------------------Non linear-------------------------------------------//

template<class lv, class rv, class op>
struct evaluator<Binary_Expression<lv, rv, op>, std::enable_if_t<is_nonlinear_op<op>()>> {
    static constexpr bool entirely_blas_expr = false;
    static constexpr bool partial_blas_expr = false;
    static constexpr bool nested_blas_expr = evaluator<lv>::nested_blas_expr || evaluator<rv>::nested_blas_expr;
    static constexpr bool requires_greedy_eval = evaluator<lv>::requires_greedy_eval || evaluator<rv>::requires_greedy_eval;


    template<class core, int a, int b, class Allocator>
    static auto linear_evaluation(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator&) {
        return branch;
    }


    struct left_trivial_injection {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
            auto left = evaluator<lv>::injection(branch.left, tensor, alloc);
            auto right = branch.right;
            return make_bin_expr<op>(left, right);
        }
    };
    struct right_trivial_injection {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
            auto left = branch.left;
            auto right = evaluator<rv>::injection(branch.right, tensor, alloc);
            return make_bin_expr<op>(left, right);
        }
    };
    struct left_nontrivial_injection {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
            auto left = evaluator<lv>::injection(branch.left, tensor, alloc);
            auto right = branch.right; //rv
            return make_bin_expr<op>(left, right);
        }
    };
    struct right_nontrivial_injection {
        template<class core, int a, int b, class Allocator>
        static auto function(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
            auto left = branch.left; //lv
            auto right = evaluator<rv>::injection(branch.right, tensor, alloc);
            return make_bin_expr<op>(left, right);
        }
    };

    template<class core, int a, int b, class Allocator>
    static auto injection(const Binary_Expression<lv, rv, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
            //dont need to update injection
            //trivial injection left_hand side (we attempt to prefer trivial injections opposed to non-trivial)
            using impl  =
                std::conditional_t<evaluator<lv>::partial_blas_expr,         left_trivial_injection,
                std::conditional_t<evaluator<rv>::partial_blas_expr,         right_trivial_injection,
                std::conditional_t<evaluator<lv>::nested_blas_expr,     left_nontrivial_injection,
                std::conditional_t<evaluator<rv>::nested_blas_expr,     right_nontrivial_injection, void>>>>;

            return impl::function(branch, tensor, alloc);

    }

    template<class allocator>
    static auto temporary_injection(const Binary_Expression<lv,rv,op>& branch, allocator& alloc) {
    	auto left  = evaluator<lv>::template temporary_injection<allocator>(branch.left, alloc);
    	auto right = evaluator<rv>::template temporary_injection<allocator>(branch.right, alloc);
    	return make_bin_expr<op>(left, right);
    }

    template<class Allocator>
    static void deallocate_temporaries(const Binary_Expression<lv, rv, op>& branch, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("Binary NonLinear: DEALLOCATE TEMPORARIES");
        evaluator<lv>::deallocate_temporaries(branch.left, alloc);
        evaluator<rv>::deallocate_temporaries(branch.right, alloc);
    }
};




//--------------Unary Expression---------------------------------------------------------------------//


template<class array_t, class op>
struct evaluator<Unary_Expression<array_t, op>>
{
    static constexpr bool entirely_blas_expr 	= false;
    static constexpr bool partial_blas_expr 	= false;
    static constexpr bool nested_blas_expr 		= evaluator<array_t>::nested_blas_expr;
    static constexpr bool requires_greedy_eval 	= evaluator<array_t>::requires_greedy_eval;

    template<class core, int a, int b, class Allocator>
    static auto linear_evaluation(const Unary_Expression<array_t, op>& branch, injector<core, a, b> tensor, Allocator&) {
        return branch;
    }
    template<class core, int a, int b, class Allocator>
    static auto injection(const Unary_Expression<array_t, op>& branch, injector<core, a, b> tensor, Allocator& alloc) {
        auto array =  evaluator<array_t>::injection(branch.array, tensor, alloc);
        return Unary_Expression<decltype(array), op>(array);
    }

    template<class allocator>
    static auto temporary_injection(const Unary_Expression<array_t, op>& branch, allocator& alloc) {

    	auto expr = evaluator<array_t>::template temporary_injection<allocator>(branch.array, alloc);
    	return Unary_Expression<std::decay_t<decltype(expr)>, op>(expr);

    }
    template<class Allocator>
     static void deallocate_temporaries(const Unary_Expression<array_t, op>& branch, Allocator& alloc) {
    	BC_TREE_OPTIMIZER_STDOUT("Unary Expression: Deallocate Temporaries");
        evaluator<array_t>::deallocate_temporaries(branch.array, alloc);
    }
};




}
}
}



#endif /* PTE_ARRAY_H_ */