/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef BLACKCAT_TENSOR_ALIASES_H_
#define BLACKCAT_TENSOR_ALIASES_H_

#include "common.h"
#include "expression_base.h"
#include "tensor_base.h"
#include "tensor_mask_proxy.h"
#include "../allocator.h"
#include "tensor_static_functions.h"

namespace bc {
namespace tensors {
namespace detail {

template<class T>
using default_allocator = bc::Allocator<T, default_system_tag_t>;

template<int X>
using default_shape = bc::Shape<X>;

} //end of ns detail
} //end of ns tensors

template<
		int dim,
		class ValueType,
		class Allocator=tensors::detail::default_allocator<ValueType>>
using Tensor = bc::tensors::Tensor_Base<
	bc::tensors::exprs::Array<
		bc::Shape<dim>,
		ValueType,
		Allocator>>;

template<
		class ValueType,
		class Allocator=tensors::detail::default_allocator<ValueType>>
using Scalar = Tensor<0, ValueType, Allocator>;

template<
		class ValueType,
		class Allocator=tensors::detail::default_allocator<ValueType>>
using Vector = Tensor<1, ValueType, Allocator>;

template<
		class ValueType,
		class Allocator=tensors::detail::default_allocator<ValueType>>
using Matrix = Tensor<2, ValueType, Allocator>;

template<
		class ValueType,
		class Allocator=tensors::detail::default_allocator<ValueType>>
using Cube = Tensor<3, ValueType, Allocator>;

template<
		class ValueType,
		 class Allocator=tensors::detail::default_allocator<ValueType>>
using VecList = bc::tensors::Tensor_Base<bc::tensors::exprs::Vector<ValueType, Allocator>>;

} //end of ns BC

#endif /* TENSOR_ALIASES_H_ */
