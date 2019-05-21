/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BC_INTERNAL_BASE_H_
#define BC_INTERNAL_BASE_H_

#include "Common.h"
#include "Shape.h"

namespace BC {
namespace exprs {

template<class derived>
class Expression_Template_Base : BC_Type {

    static constexpr int  DIMS = derived::DIMS;

    BCINLINE const derived& as_derived() const { return static_cast<const derived&>(*this); }
    BCINLINE       derived& as_derived()       { return static_cast<      derived&>(*this); }

public:

    BCINLINE const auto& internal() const { return as_derived(); }
    BCINLINE       auto& internal()       { return as_derived(); }

    operator       derived&()       { return as_derived(); }
    operator const derived&() const { return as_derived(); }

    BCINLINE Expression_Template_Base() {
		static_assert(std::is_trivially_copy_constructible<derived>::value, "INTERNAL_TYPES TYPES MUST BE TRIVIALLY COPYABLE");
		static_assert(std::is_trivially_copyable<derived>::value, "INTERNAL_TYPES MUST BE TRIVIALLY COPYABLE");
		static_assert(!std::is_same<void, typename derived::value_type>::value, "INTERNAL_TYPES MUST HAVE A 'using value_type = some_Type'");
		static_assert(!std::is_same<decltype(std::declval<derived>().inner_shape()), void>::value, "INTERNAL_TYPE MUST DEFINE inner_shape()");
		static_assert(!std::is_same<decltype(std::declval<derived>().block_shape()), void>::value, "INTERNAL_TYPE MUST DEFINE block_shape()");
		static_assert(std::is_same<decltype(std::declval<derived>().rows()), int>::value, "INTERNAL_TYPE MUST DEFINE rows()");
		static_assert(std::is_same<decltype(std::declval<derived>().cols()), int>::value, "INTERNAL_TYPE MUST DEFINE cols()");
		static_assert(std::is_same<decltype(std::declval<derived>().dimension(0)), int>::value, "INTERNAL_TYPE MUST DEFINE dimension(int)");
		static_assert(std::is_same<decltype(std::declval<derived>().block_dimension(0)), int>::value, "INTERNAL_TYPE MUST DEFINE block_dimension(int)");
		static_assert(std::is_same<bool, std::decay_t<decltype(derived::copy_constructible)>>::value, "Internal Types must define 'static constexpr bool copy_constructible'");
		static_assert(std::is_same<bool, std::decay_t<decltype(derived::move_constructible)>>::value, "Internal Types must define 'static constexpr bool move_constructible'");
		static_assert(std::is_same<bool, std::decay_t<decltype(derived::copy_assignable)>>::value, "Internal Types must define 'static constexpr bool copy_assignable'");
		static_assert(std::is_same<bool, std::decay_t<decltype(derived::move_assignable)>>::value, "Internal Types must define 'static constexpr bool move_assignable'");
		static_assert(std::is_same<int, std::decay_t<decltype(derived::DIMS)>>::value, "Internal Types must define 'static constexpr int DIMS'");
		static_assert(std::is_same<int, std::decay_t<decltype(derived::ITERATOR)>>::value, "Internal Types must define 'static constexpr int ITERATOR'");
    }

    void deallocate() const {}

};

template<class derived>
struct Expression_Base
        : Expression_Template_Base<derived>,
          BC_Expr {

            static constexpr bool copy_constructible = false;
            static constexpr bool move_constructible = false;
            static constexpr bool copy_assignable    = false;
            static constexpr bool move_assignable    = false;
        };

template<class Derived, int Dimension>
struct Array_Base : Expression_Template_Base<Derived>, BC_Array {

	BCHOT operator const auto*() const { return static_cast<const Derived&>(*this).memptr(); }
	BCHOT operator       auto*()       { return static_cast<      Derived&>(*this).memptr(); }

    BCINLINE Array_Base() {
		static_assert(!std::is_same<void, decltype(std::declval<Derived>().memptr())>::value, "Array types must define memptr");
    }


};

}
}


#endif /* BC_INTERNAL_BASE_H_ */
