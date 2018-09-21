/*
 * FeedForward.cu
 *
 *  Created on: Jan 28, 2018
 *      Author: joseph
 */

#ifndef OUTPUT_CU
#define OUTPUT_CU

#include "../../BC_Internals/Layers/Layer_Base.h"

namespace BC {
namespace NN {

//template<class derived>
struct InputLayer : Layer_Base {

	InputLayer(int inputs, int outputs) : Layer_Base(inputs, outputs) {}

	mat_view x;

	const auto& forward_propagation() {
		return x;
	}

	void back_propagation() {
	}


	void set_batch_size(int x) {}
	void update_weights() {}
	void clear_stored_gradients() {}
	void write(std::ofstream& os) {}
	void read(std::ifstream& is) {}
	void set_learning_rate(fp_type learning_rate) {}

	auto& inputs()  { return x; }
	auto& deltas()  { return NULL_TENSOR; }
	auto& outputs() { return x; }
	auto& weights()	{ return NULL_TENSOR; }
	auto& bias()	{ return NULL_TENSOR; }

	template<class tensor, class deltas> void set_activation(tensor&,deltas&) {};
	template<class tensor> void set_weight(tensor&) {};
	template<class tensor> void set_bias(tensor&) {};

};
}
}
#endif /* FEEDFORWARD_CU_ */