/*
 * Determiners.h
 *
 *  Created on: Feb 25, 2018
 *      Author: joseph
 */

#ifndef DETERMINERS_H_
#define DETERMINERS_H_

#include "BC_Expressions/BlackCat_Internal_Definitions.h"
#include <type_traits>
namespace BC {


template<bool cond, class a, class b> using ifte = typename std::conditional<cond, a, b>::type;

template<class> struct shell_of;

template<template<class...> class shell, class... params>
struct shell_of<shell<params...>> {
	template<class... T> using type = shell<T...>;
};
template<class> class Tensor_Core;
template<class> class Tensor_Slice;
template<class> class Tensor_Scalar;

template<class var, class... lst> struct isOneOf { static constexpr bool conditional = false; };
template<class var, class... lst> struct isOneOf<var,var,lst...> { static constexpr bool conditional = true; };
template<class var, class front, class... lst> struct isOneOf<var,front,lst...> { static constexpr bool conditional = isOneOf<var, lst...>::conditional; };

template<class T> struct isCore { static constexpr bool conditional = isOneOf<T,double,float,int,char>::conditional; };
template<template<class> class T, class U> struct isCore<T<U>>
{ static constexpr bool conditional = isOneOf<T<U>,Tensor_Core<U>, Tensor_Slice<U>, Tensor_Scalar<U>>::conditional; };

template<class T> struct isCorePure { static constexpr bool conditional = isOneOf<T,double,float,int,char>::conditional; };
template<template<class> class T, class U> struct isCorePure<T<U>>
{ static constexpr bool conditional = isOneOf<T<U>,Tensor_Core<U>>::conditional; };

template<class,class> class DISABLED;
template<class,class> class Scalar;
template<class,class> class Vector;
template<class,class> class Matrix;
template<class,class> class Cube;
template<class> struct Tensor_Core;

template<int> struct base;

template<> struct base<0> { template<class t, class m> using type = Scalar<t,m>;  template<class t, class m> using slice = DISABLED<t, m>; };
template<> struct base<1> { template<class t, class m> using type = Vector<t, m>; template<class t,class m> using slice = Scalar<t, m>; };
template<> struct base<2> { template<class t, class m> using type = Matrix<t, m>; template<class t,class m> using slice = Vector<t, m>; };
template<> struct base<3> { template<class t, class m> using type = Cube<t, m>;   template<class t,class m> using slice = Matrix<t, m>; };

template<int a ,int b = a, class = void> struct Rank;
template<class> struct ranker;
template<class a, class b> struct ranker<Scalar<a,b>> { static constexpr int value = 0; using type = Rank<value, value>;  };
template<class a, class b> struct ranker<Vector<a,b>> { static constexpr int value = 1; using type = Rank<value, value>;  };
template<class a, class b> struct ranker<Matrix<a,b>> { static constexpr int value = 2; using type = Rank<value, value>;  };
template<class a, class b> struct ranker<Cube<a,b>>   { static constexpr int value = 3; using type = Rank<value, value>;  };

template<class T> struct lst {
	using front = T;
	using last = T;
};
template<template<class...> class LIST, class T, class... V>
struct lst<LIST<T, V...>>  {
	using front = T;
	using last = T;
};
template<template<class...> class LIST, class T, class V>
struct lst<LIST<T, V>>  {
	using front = T;
	using last = V;
};

template<template<class...> class LIST, class T, class n, class... V>
struct lst<LIST<T, n, V...>> {
	using front = T;
	using last = typename lst<LIST<n,V...>>::last;
};


template<class T> using _scalar = typename MTF::determine_scalar<T>::type;
template<class T> using _mathlib = typename lst<T>::last;
template<class T> using _ranker  = typename ranker<T>::type;
template<class T> static constexpr int _rankOf  = ranker<T>::value;

///DET FUNCTOR
template<class> struct determine_functor;
template<template<class...> class tensor, class functor, class... set>
struct determine_functor<tensor<functor, set...>>{

	using derived = tensor<functor,set...>;
	using fscal = functor;
	using type = ifte<MTF::isPrimitive<functor>::conditional, Tensor_Core<derived>, functor>;
};

template<class T>
using _functor = typename determine_functor<T>::type;
template<class T> using _fscal = typename determine_functor<T>::fscal;

///DET EVALUATION
template<class> struct determine_evaluation;
template<template<class...> class tensor, class functor, class... set>
struct determine_evaluation<tensor<functor, set...>>{
	using derived = tensor<functor,set...>;

	using type = ifte<MTF::isPrimitive<functor>::conditional || isCore<functor>::conditional,
			derived&,
			tensor<Tensor_Core<derived>>>;
};
template<class T> using _evaluation = typename determine_evaluation<T>::type;


}



#endif /* DETERMINERS_H_ */