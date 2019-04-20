/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INJECTION_INFO_H_
#define INJECTION_INFO_H_


namespace BC {
namespace exprs {
namespace tree {


template<class tensor_core, int alpha_modifier_ = 1, int beta_modifier_= 0>
struct injector {

	static constexpr BC::size_t  ALPHA = alpha_modifier_;
	static constexpr BC::size_t  BETA = beta_modifier_;
    injector(tensor_core& array_) : array(array_) {}

    tensor_core array;

    operator const tensor_core& () const { return array; }
    operator       tensor_core& ()       { return array; }

    const tensor_core& data() const { return array; }
          tensor_core& data()       { return array; }
};

//entirely_blas_expr -- detects if the tree is entirely +/- operations with blas functions, --> y = a * b + c * d - e * f  --> true, y = a + b * c --> false
template<class op, bool prior_eval, class core, int a, int b>//only apply update if right hand side branch
auto update_injection(injector<core,a,b> tensor) {
    static constexpr int alpha =  a * BC::oper::operation_traits<op>::alpha_modifier;
    static constexpr int beta  = prior_eval ? 1 : 0;
    return injector<core, alpha, beta>(tensor.data());
}

template<int a, int b, class core>
auto make_injection(core c) {
	return injector<core, a, b>(c);
}


}
}
}


#endif /* INJECTION_INFO_H_ */