#ifndef RECURRENT_TEST
#define RECURRENT_TEST
#include "../BlackCat_NeuralNetworks.h"
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <random>
using BC::NN::vec;
using BC::NN::scal;
using BC::NN::mat;

namespace BC {
namespace NN {
namespace FunctionMatcher {
typedef std::vector<mat> internal;

//CPU only
mat fixed_sin_set(int n) {
	mat sins(1, n);

	float seed = rand();
	while (seed > 100)
		seed /= 10;

	for (int i = 0; i < n; ++i) {
		seed += 3.14 / 8;
		sins(i) = (std::sin(seed) + 1) / 2;
	}
	return sins;
}



int test() {

	const int TRAINING_ITERATIONS = 100;
	const int NUMB_EXAMPLES = 100;
	const int TESTS  = 10;
	NeuralNetwork<FeedForward, GRU, FeedForward> network(1, 125, 40, 1);
	network.setLearningRate(.003);

	internal inputs(NUMB_EXAMPLES);
	for (int i = 0; i < NUMB_EXAMPLES; ++i){
		int length = (rand() % 10) + 3;
		inputs[i] = fixed_sin_set(length);
	}

//	std::ifstream is("NN_adj_sin.bc");
//	network.read(is);

	//Train
	float t;
	t = omp_get_wtime();
	printf("\n Calculating... BC_NN training time \n");
	std::cout << " training..." << std::endl;

	for (int i = 0; i < TRAINING_ITERATIONS; ++i) {
		if (i % 10 == 0)
		std::cout << " iteration =  " << i << std::endl;
		for (int j = 0; j < inputs.size(); ++j) {
			for (int l = 0; l < inputs[j].cols()-1; ++l)
				network.forwardPropagation(inputs[j][l]);

			for (int l = inputs[j].cols()-1; l > 0; --l)
				network.backPropagation(inputs[j][l]);

			network.updateWeights();
			network.clearBPStorage();
		}
	}

	for (int i = 0; i < 15; ++i) {
		std::cout << std::endl << std::endl;
		std::cout << " set ------------------" << std::endl;
		for (int j = 0; j < 2; ++j) {
			vec x = inputs[i][j + 1] - network.forwardPropagation_Express(inputs[i][j]);
			std::cout << " error = "; x.print();
		}
		vec out = network.forwardPropagation_Express(inputs[i][2]);
		for (int j = 3; j < inputs[i].cols() - 1; ++j) {

			//feeding its own output
			out = network.forwardPropagation_Express(out);
			vec x = out - inputs[i][j + 1];
			std::cout << " predicting into future---Hypothesis/Output = " << out(0) << "|" << inputs[i][j + 1](0) << "error = "; x.print();
		}
	}


//	std::ofstream os("NN_adj_sin.bc");
//	network.write(os);
}


}
}
}
#endif
