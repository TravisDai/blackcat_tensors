/*
 * Caffe.h
 *
 *  Created on: Oct 31, 2019
 *      Author: joseph
 */

#ifndef BLACKCAT_CAFFE_CAFFE_H_
#define BLACKCAT_CAFFE_CAFFE_H_

#include "img2col.cu"
#include "img2col.h"
#include "maxpooling.h"
#include "maxpooling.cu"

///THIS IS NOT A CAFFE FILE --
///JOSEPH JASPERS IS THE AUTHOR OF THIS FILE

namespace bc {

template<
	class Stream,
	class Indexes,
	class Image,
	class ImageOut>
void max_pooling_forward(
		Stream stream,
		Image image,
		ImageOut out,
		Indexes mask,
		bc::Dim<2> krnl_shape,
		bc::Dim<2> padding = bc::Dim<2>().fill(0),
		bc::Dim<2> strides = {-1,-1}) {

	if (strides == Dim<2>{ -1,-1 })
		strides = krnl_shape;

	BC_ASSERT((out.inner_shape().template subdim<0,2>() ==
			(image.inner_shape().template subdim<0,2>() + padding*2)/strides),
			"ASSERT MAX_POOLING_FORWARD"
			"\nout.inner_shape() == "
			"(image.inner_shape() + padding)/strides");
	BC_ASSERT(out.dimension(2) == image.dimension(2), "numb channels must be the same");
	BC_ASSERT(out.dimension(3) == image.dimension(3), "batch size must be the same");

	using system_tag = typename Stream::system_tag;

	stream.enqueue([=]() {
		bc::caffe::MaxPoolForward(
				system_tag(),
				image.data(),
				image.dimension(3),
				image.dimension(2),
				image.dimension(0), image.dimension(1),
				out.dimension(0), out.dimension(1),
				krnl_shape[0], krnl_shape[1],
				strides[0], strides[1],
				padding[0], padding[1],
				out.data(), mask.data());
	});
}


template<
	class Stream,
	class Indexes,
	class Image,
	class ImageOut>
void max_pooling_backward(
		Stream stream,
		Image image,         //output delta (not initialized)
		ImageOut delta,      //delta from upper layer
		Indexes mask,        //indicies of delta from upper layer
		bc::Dim<2> krnl_shape,
		bc::Dim<2> padding = bc::Dim<2>().fill(0),
		bc::Dim<2> strides = {-1,-1})
{

	static_assert(std::is_same<
			int,
			typename Indexes::value_type>::value,
			"Mask must be int");

	static_assert(std::is_same<
			typename Image::value_type,
			typename ImageOut::value_type>::value,
			"Delta/Image value_type must be the same");


	if (strides == Dim<2>{ -1,-1 })
		strides = krnl_shape;

	BC_ASSERT((delta.inner_shape().template subdim<0,2>() ==
			(image.inner_shape().template subdim<0,2>() + padding*2)/strides),
			"ASSERT MAX_POOLING_FORWARD"
			"\nout.inner_shape() == "
			"(image.inner_shape() + padding)/strides");
	BC_ASSERT(delta.dimension(2) == image.dimension(2), "numb channels must be the same");
	BC_ASSERT(delta.dimension(3) == image.dimension(3), "batch size must be the same");

	using system_tag = typename Stream::system_tag;

	stream.enqueue([=]() {
		bc::caffe::MaxPoolBackward(
				system_tag(),
				delta.data(), mask.data(),
				image.dimension(3), image.dimension(2),
				image.dimension(0), image.dimension(1),
				delta.dimension(0), delta.dimension(1),
				krnl_shape[0], krnl_shape[1],
				strides[0], strides[1],
				padding[0], padding[1],
				image.data());
	});
}

template<
	class Stream,
	class ColumnImage,
	class Image>
void im2col(
		Stream stream,
		ColumnImage col_image,
		Image image,
		bc::Dim<3> krnl_shape,
		bc::Dim<2> padding = bc::Dim<2>(),
		bc::Dim<2> strides = bc::Dim<2>().fill(1),
		bc::Dim<2> dilation = bc::Dim<2>().fill(1)) {

	static_assert(ColumnImage::tensor_dimension == 2,
			"ColumnImage must be a matrix");
	static_assert(Image::tensor_dimension == 3,
			"2d Convolution expects a 3d-image input");

	using system_tag = typename Stream::system_tag;

	stream.enqueue([=]() {
		 bc::caffe::im2col(
				system_tag(),
				image.data(),
				image.dimension(2),
				image.dimension(1), image.dimension(0),
				krnl_shape[1], krnl_shape[0],
				padding[1], padding[0],
				strides[1], strides[0],
				dilation[1], dilation[0],
				col_image.data());
	});
}


template<
	class Stream,
	class ColumnImage,
	class Image>
void col2im(
		Stream stream,
		ColumnImage col_image,
		Image image,
		bc::Dim<3> krnl_shape,
		bc::Dim<2> padding = bc::Dim<2>(),
		bc::Dim<2> strides = bc::Dim<2>().fill(1),
		bc::Dim<2> dilation = bc::Dim<2>().fill(1)) {

	static_assert(ColumnImage::tensor_dimension == 2,
			"ColumnImage must be a matrix");
	static_assert(Image::tensor_dimension == 3,
			"2d Convolution expects a 3d-image input");

	using system_tag = typename Stream::system_tag;

	stream.enqueue([=]() {
		 bc::caffe::col2im(
				system_tag(),
				col_image.data(),
				image.dimension(2),
				image.dimension(1), image.dimension(0),
				krnl_shape[1], krnl_shape[0],
				padding[1], padding[0],
				strides[1], strides[0],
				dilation[1], dilation[0],
				image.data());
	});
}


template<
	class Stream,
	class ColumnImage,
	class Image,
	int NumAxis>
void im2col_nd(
		Stream stream,
		ColumnImage col_image,
		Image image,
		bc::Dim<NumAxis> krnl_shape,
		bc::Dim<NumAxis> padding = bc::Dim<NumAxis>(),
		bc::Dim<NumAxis> strides = bc::Dim<NumAxis>().fill(1),
		bc::Dim<NumAxis> dilation = bc::Dim<NumAxis>().fill(1)) {

	constexpr bool is_batched = Image::tensor_dimension == NumAxis + 1;
	static_assert(ColumnImage::tensor_dimension == 2 + is_batched,
			"Invalid ColumnImage dimension");
	static_assert(Image::tensor_dimension == NumAxis + is_batched,
			"Invalid Image dimension");

	using system_tag = typename Stream::system_tag;

	//assume non-strided data (must be packed format)
	auto img_shape = image.get_shape().inner_shape().reverse();
	auto col_shape = col_image.get_shape().inner_shape().reverse();

	stream.enqueue([=]() {
		 bc::caffe::im2col_nd(
				system_tag(),
				image.data(),
				NumAxis,
				img_shape.data(),
				col_shape.data(),
				krnl_shape.reverse().data(),
				padding.reverse().data(),
				strides.reverse().data(),
				dilation.reverse().data(),
				col_image.data());
	});
}

}

#endif /* CAFFE_H_ */
