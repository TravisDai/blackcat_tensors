/*
 * Scalar.h
 *
 *  Created on: Jan 6, 2018
 *      Author: joseph
 */

#ifndef SCALAR_H_
#define SCALAR_H_
#include "TensorBase.h"
#include "../BC_MetaTemplateFunctions/Simple.h"
#include "../BC_MetaTemplateFunctions/Adhoc.h"
#include <type_traits>

namespace BC {

template<class T, class Mathlib>
class Scalar : public TensorBase<Scalar<T, Mathlib>> {

	using parent_class = TensorBase<Scalar<T, Mathlib>>;
	template<class, class> friend class Vector;

public:
	static constexpr int RANK() { return 0; }

	using parent_class::operator=;
	using parent_class::operator();

	Scalar() {}
	Scalar(const Scalar&& t) : parent_class(t) 		{}
	Scalar(		 Scalar&& t) : parent_class(t) 		{}
	Scalar(const Scalar&  t) : parent_class(t) 		{}

	Scalar& operator =(const Scalar&  t) { return parent_class::operator=(t); }
	Scalar& operator =(const Scalar&& t) { return parent_class::operator=(t); }
	Scalar& operator =(	     Scalar&& t) { return parent_class::operator=(t); }
	template<class U>
	Scalar& operator =(const Scalar<U, Mathlib>& t) { return parent_class::operator=(t); }
	Scalar& operator =(_scalar<T> scalar) { Mathlib::DeviceToHost(this->data().core(), &scalar, 1); return *this; }

	using _shape = std::vector<int>;

	template<bool var, class a, class b>
	using ifte = std::conditional_t<var, a, b>;


	Scalar(_scalar<T>* param): parent_class(param) {}

	//this how to do constexpr if -esque things inside of an initialization list in a constructor
	struct sendParam { template<class u>  	static auto impl(const u& param) { return param; }};
	struct sendNull { template<class u>  	static auto impl(const u& param) { return _shape(); }};
	struct htd { template<class... u>  		static void impl(const u&... param) { Mathlib::HostToDevice(param...); }};
	struct voider { template<class... u>  	static void impl(const u&... parma) { }};

	//we use this ifte to differentiate between primitive initializations (in the case we want to... Scalar<double>(2.0) //make a scalar with value 2
	//and...... Scalar<double>(unary_Expression<of some types>); In the first instance we enable Tensor_Core to call its default constructor
	//			in this second instance we initiate TensorBase's functor type with U.
	struct DISABLE;
	explicit Scalar(const ifte<MTF::isPrimitive<T>::conditional, DISABLE, const T&> value) : parent_class(value) {}
	Scalar(_scalar<T> value) {
		Mathlib::HostToDevice((_scalar<T>*)this->data(), &value, 1);
	}
//
	//This is the same constructor as above (for non-genuine tensors) but we mandate 2 parameters so the compiler doesn't confuse it with the above constructor
	template<class var1, class var2, class... params>
	explicit Scalar(const var1& v1, const var2& v2, const params&... p) : parent_class(v1, v2, p...) {}

};


}



#endif /* SCALAR_H_ */
