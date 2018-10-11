
/*
 * Tensor_Base.h
 *
 *  Created on: Jan 6, 2018
 *      Author: joseph
 */

#ifndef TENSOR_BASE_H_
#define TENSOR_BASE_H_

#include "Tensor_Common.h"
#include "Tensor_Operations.h"
#include "Tensor_Utility.h"
#include "Tensor_Shaping.h"
#include "Tensor_Functions.h"

#include "Expression_Templates/Array.h"
#include "Expression_Templates/Array_View.h"
#include "Expression_Templates/Array_Shared.h"


namespace BC {

template<class internal_t>
class Tensor_Base :
		public internal_t,
		public module::Tensor_Operations<Tensor_Base<internal_t>>,
		public module::Tensor_Functions<Tensor_Base<internal_t>>,
		public module::Tensor_Utility<Tensor_Base<internal_t>>,
		public module::Tensor_Shaping<Tensor_Base<internal_t>> {

protected:

	using parent        = internal_t;
	using self          = Tensor_Base<internal_t>;
	using operations    = module::Tensor_Operations<Tensor_Base<internal_t>>;
	using utility       = module::Tensor_Utility<Tensor_Base<internal_t>>;
	using shaping       = module::Tensor_Shaping<Tensor_Base<internal_t>>;

	template<class> friend class Tensor_Base;
	using internal_t::internal_t;

public:

	using internal_t::DIMS; //required
	using scalar_t	= typename internal_t::scalar_t;
	using mathlib_t	= typename internal_t::mathlib_t;
	using operations::operator=;
	using shaping::operator[];
	using shaping::operator();

	using move_parameter        = std::conditional_t<BC_array_move_constructible<internal_t>(),       self&&, 	BC::DISABLED<0>>;
	using copy_parameter        = std::conditional_t<BC_array_copy_constructible<internal_t>(), const self&,  	BC::DISABLED<1>>;
	using move_assign_parameter = std::conditional_t<BC_array_move_assignable<internal_t>(), 	      self&&, 	BC::DISABLED<0>>;
	using copy_assign_parameter = std::conditional_t<BC_array_copy_assignable<internal_t>(), 	const self&,  	BC::DISABLED<1>>;

	Tensor_Base() = default;
	Tensor_Base(const parent&  param) : internal_t(param) {}
	Tensor_Base( 	  parent&& param) : internal_t(param) {}

	template<class U> Tensor_Base(const Tensor_Base<U>&  tensor) : internal_t(tensor.internal()) {}
	template<class U> Tensor_Base(	    Tensor_Base<U>&&  tensor) : internal_t(tensor.internal()) {}


	Tensor_Base(copy_parameter tensor) {
		this->copy_init(tensor);
	}

	Tensor_Base(move_parameter tensor) {
		this->swap_array(tensor);
		this->swap_shape(tensor);
	}
	Tensor_Base& operator =(move_assign_parameter tensor) {
		this->swap_shape(tensor);
		this->swap_array(tensor);
		return *this;
	}
	Tensor_Base& operator =(copy_assign_parameter tensor) {
		 operations::operator=(tensor);
		 return *this;
	}

	Tensor_Base(scalar_t scalar) {
		static_assert(DIMS() == 0, "SCALAR_INITIALIZATION ONLY AVAILABLE TO SCALARS");
		this->fill(scalar);
	}

	~Tensor_Base() {
		this->destroy();
	}

	 const parent& internal() const { return static_cast<const parent&>(*this); }
	 	   parent& internal() 	  	{ return static_cast<	   parent&>(*this); }
};

}

#endif /* TENSOR_BASE_H_ */
