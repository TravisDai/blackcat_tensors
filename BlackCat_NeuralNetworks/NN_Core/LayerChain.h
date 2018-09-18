
#ifndef BLACKCAT_TUPLE
#define BLACKCAT_TUPLE

#include "Layers/InputLayer.h"
#include "Layers/OutputLayer.h"
namespace BC {
namespace NN {

template<int index, class derived, class...>
struct LayerChain;
class OutputLayer;
class InputLayer;

//TAIL
template<int index, class derived>
struct LayerChain<index, derived, OutputLayer> {

	using self = LayerChain<index, derived, OutputLayer>;
	using type = OutputLayer;

	OutputLayer layer;

	LayerChain(int x) : layer(x) {}
	const auto& head() const { return prev().head(); }
		  auto& head()  	 { return prev().head(); }
	const auto& tail() const { return *this; }
		  auto& tail() 		 { return *this; }
	const auto& prev() const { return static_cast<const derived&>(*this); }
		  auto& prev()    	 { return static_cast<		derived&>(*this); }

	template<class T> const auto& fp(const T& tensor) { return layer.forward_propagation(tensor); }
	template<class T> const auto& bp(const T& tensor) { return this->prev().bp(layer.back_propagation(tensor)); }
	template<class function> void for_each(function f) { f(layer); };


};

//BODY
template<int index, class derived,class front, class... lst>
struct LayerChain<index, derived, front, lst...>
: LayerChain<index + 1, LayerChain<index, derived, front, lst...>, lst...> {

	using self 		= LayerChain<index, derived, front, lst...>;
	using parent 	= LayerChain<index + 1, self, lst...>;
	using type 		= front;

	front layer;

	template<class param, class... integers>
	LayerChain(param i, param o, integers... dims) :  parent(o, dims...), layer(i,o) {}

	const auto& head() const { return prev().head(); }
		  auto& head() 		 { return prev().head(); }
	const auto& tail() const { return next().tail(); }
		  auto& tail() 		 { return next().tail(); }
	const auto& next() const { return static_cast<const parent&>(*this); }
		  auto& next()    	 { return static_cast<		parent&>(*this); }
	const auto& prev() const { return static_cast<const derived&>(*this); }
		  auto& prev()    	 { return static_cast<		derived&>(*this); }

	template<class T> const auto& fp(const T& tensor) { return this->next().fp(layer.forward_propagation(tensor)); }
	template<class T> const auto& bp(const T& tensor) { return this->prev().bp(layer.back_propagation(tensor)); }

	template<class function> void for_each(function f) {
		f(layer);
		next().for_each(f);
	}
};

//HEAD
template<class... lst>
struct Chain : public LayerChain<0, Chain<lst...>, lst...>{

	using self = Chain<lst...>;
	using parent = LayerChain<0, self, lst...>;

	template<class... integers>
	Chain(int x, integers... dims) : parent(x, x, dims...) {} //first layer is always input layer (so we double x)

	template<class T>
	const auto& backprop(const T& tensor_expected) { return this->tail().bp(tensor_expected); }

	void read(std::ifstream& is) 		{ this->for_each([&](auto& layer) { layer.read(is); 	});}
	void write(std::ifstream& os) 		{ this->for_each([&](auto& layer) { layer.write(os); 	});}
	void set_batch_size(int x) 			{ this->for_each([&](auto& layer) { layer.set_batch_size(x);	});}
	void update_weights() 				{ this->for_each([ ](auto& layer) { layer.update_weights();		});}
	void setLearningRate(fp_type lr) 	{ this->for_each([&](auto& layer) { layer.setLearningRate(lr); 	});}
	void clear_stored_delta_gradients() { this->for_each([ ](auto& layer) { layer.clear_stored_delta_gradients(); }); }



	//required terminating methods
	template<class T>
	const auto& bp(const T& dx) { return dx; }
	auto& head() { return this->data(); }
};
}
}
#endif