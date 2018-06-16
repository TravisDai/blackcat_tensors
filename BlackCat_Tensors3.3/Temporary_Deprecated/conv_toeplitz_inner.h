///*
// * conv_toeplitz.h
// *
// *  Created on: May 30, 2018
// *      Author: joseph
// */
//
//#ifndef CONV_TOEPLITZ_INNER_H_
//#define CONV_TOEPLITZ_INNER_H_
//
//namespace BC {
//
//template<int movement, int stride = 1>
//struct conv_toeplitz_inner_gen {
//	template<class output, class tensor, class krnl_shape, class... indicies>
//	static void impl(output& out, const tensor& image, const krnl_shape& k, indicies... ints) {
//		for (int i = 0; i < out.outer_dimension(); i ++) {
//			auto slice = out.slice(i);
//			conv_toeplitz_inner_gen<movement - 1, stride>::impl(slice, image, k, ints..., i);
//		}
//	}
//};
//template<int stride>
//struct conv_toeplitz_inner_gen<0, stride> {
//	template<class output, class tensor, class krnl_shape, class... indicies>
//	static void impl(output& out, const tensor& image, const krnl_shape& k,  indicies... ints) {
//		////utilize a non-barrier copy method
//		out =** chunk(image)(ints...)(k);
//	}
//};
//
//
//template<int movement, int stride = 1,  class tensor, class krnl_shape>
//static auto conv_toeplitz_inner(const tensor& image, const krnl_shape& k) {
//	using scalar = _scalar<tensor>;
//	using ml = _mathlib<tensor>;
//	constexpr int new_dims = movement + krnl_shape::DIMS();
//
//
//	///SHAPES ARE NOT SUPPOSED TO BE CONSTRUCTED LIKE THIS, THIS IS JUST THE EASIEST WAY FOR THIS PROBLEM
//	Shape<new_dims> shape;
//	for (int i = 0; i < krnl_shape::DIMS(); ++i) {
//		shape.is()[i] = k.dimension(i);
//	}
//	for (int i =  krnl_shape::DIMS(); i < new_dims; ++i) {
//		shape.is()[i] = (image.dimension(i - krnl_shape::DIMS	()) - k.dimension(i - krnl_shape::DIMS()) + 1);
//	}
//	shape.calculate_outer_dimensions(); //initializes the outer dimensions of a shape
//	tensor_of_t<movement, scalar, ml> output(Shape<movement>(k.size(), shape.size() / k.size()));
//	auto shaped = reshape(output)(shape);
//	conv_toeplitz_inner_gen<movement, stride>::impl(shaped, image, Shape<krnl_shape::DIMS()>(k.inner_shape()));
//
//	ml::barrier();
//	return output;
//}
//
//
//
//
//}
//
//
//#endif /* CONV_TOEPLITZ_H_ */