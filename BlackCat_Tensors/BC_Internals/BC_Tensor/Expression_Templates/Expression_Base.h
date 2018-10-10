/*
 * BC_Expression_Base.h
 *
 *  Created on: Dec 11, 2017
 *      Author: joseph
 */

#ifndef EXPRESSION_BASE_H_
#define EXPRESSION_BASE_H_

#include "Internal_Type_Interface.h"
namespace BC {
namespace internal {

template<class derived>
struct Expression_Base
		: BC_internal_interface<derived>{
};

template<class derived>
struct Function_Interface
		: BC_internal_interface<derived> {
};

}
}

#endif /* EXPRESSION_BASE_H_ */
