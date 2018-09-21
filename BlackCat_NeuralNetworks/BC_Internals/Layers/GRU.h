///*
// * GRU.h
// *
// *  Created on: Sep 20, 2018
// *      Author: joseph
// */
//
//#ifndef BC_INTERNALS_LAYERS_GRU_H_
//#define BC_INTERNALS_LAYERS_GRU_H_
//
//#include "Layer_Base.h"
//
//namespace BC {
//namespace NN {
//
//struct GRU : public Layer_Base{
//public:
//
//	using Layer_Base::lr;	//the learning rate
//
//	mat dy;					//error
//	mat_shared y;					//outputs
//	mat_view x;				//inputs
//
//	mat w;			//weights
//	vec b;			//biases
//
////	mat w_gradientStorage;	//weight gradient storage
////	vec b_gradientStorage;	//bias gradient storage
//
//
//	GRU(int inputs, int outputs) :
//		Layer_Base(inputs, outputs),
//			w(outputs, inputs + outputs), //add outputs for recurrence
//			b(outputs)//,
//
////			w_gradientStorage(outputs, this->INPUTS),
////			b_gradientStorage(outputs)
//	{	}
//
//	template<class t> const auto& forward_propagation(const expr::mat<t>& x_) {
//		x = mat_view(x_);
//
//		return y = g(w * x + b);
//	}
//	template<class t> auto back_propagation(const expr::mat<t>& dy_) {
//		dy = dy_;
//		return w.t() * dy % gd(x);
//	}
//	void update_weights() {
//		w -= dy * lr * x.t();
//		b -= dy * lr;
//	}
//
//	void set_batch_size(int batch_sz) {
//		y = mat_shared(this->outputs(), batch_sz);
//		x = mat_view(this->inputs() + this->outputs(), batch_sz);
//		dy = mat(this->outputs(), batch_sz);
//	}
//
//	auto& outputs() { return y; }
//	auto& weights()	    { return w; }
//	auto& bias()		{ return b; }
//
//	template<class tensor> void set_activation(tensor& workspace) {
//		y.internal() = workspace.internal();
//	}
//
//	template<class tensor> void set_weight(tensor& workspace) {
////		w.internal() = workspace.internal().memptr();		//FIXME w = workspace.internal() //doesn't compile but should
//		w.randomize(-2,2);
//	}
//	template<class tensor> void set_bias(tensor& workspace) {
////		b = workspace.internal();
//		b.randomize(-1,1);
//	}
//
//}
//}
//
//
//
//
//#endif /* BC_INTERNALS_LAYERS_GRU_H_ */