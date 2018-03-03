#include <iostream>
#include "../BlackCat_Tensors.h"
#include "SpeedTests.h"
using BC::Vector;
using BC::Matrix;
using BC::Scalar;
using BC::Cube;

using ml = BC::CPU;
//using ml = BC::GPU;

using vec = Vector<float, ml>;
using mat = Matrix<float, ml>;
using scal = Scalar<float, ml>;
using cube = Cube<float, ml>;
//
//using vec = Vector<double, CPU>;
//using mat = Matrix<double, CPU>;

auto test() {



	mat a(3, 2);
	mat b(2 ,3);
	mat d(2, 3);
	mat e(3, 2);
	mat c(2, 2);


	Matrix<float, BC::GPU> fasd(4,4);
	fasd.randomize(-3, 3);

	fasd.printDimensions();
	fasd.print();

	std::cout << " param " << std::endl;

	a.printDimensions();

	for (int i = 0; i < 6; ++i)  {
		b(i) = i + 7;
		a(i) = i + 1;
	}


	std::cout << std::endl;

	d = a.t();
	e = b.t();


	a.print();
	b.print();
	d.print();
	e.print();

	std::cout << " simple dot product " << std::endl;
	c = d * e;
	c.print();
	///all the permutations of optimized dotproduct
	c = a.t() * b.t();
	c = a.t() * e;
	c = d * b.t();
	c = d * scal(2) * e;
	c = scal(2) * d * e;
	c = d * e * scal(2); ////This is the only version that is not accounted for (it is also the least common notation)
	c = d * scal(2) * e;

	c.print();

	scal A(2);
	scal B(2);

	c.print();

	c = a.t() * b.t();

	A.print();

	c = a.t() * A * (b.t() * A);

	c.print();

	cube cu(2,3, 4);
	cu.zero();
	cu.print();


	std::cout << " sub vector test " << std::endl;

	d.print();
	std::cout << " adding d[1] = d[1] + d[1]" << std::endl;
	d[1] = d[1] + d[1];
	d.print();
	std::cout << " adding d[1][1] = d[1][1] + d[1][1] " << std::endl;


	d[1][1] = d[1][1] + d[1][1];
	d.print();
}


#include "../BC_Extensions/BC_Correlation.h"
int main() {

//	Matrix<double> krnl(2,2);
//	Matrix<double> img(3,3);
//	Matrix<double> out(4,4);
//	Matrix<double> out2(2, 2);
//
//	for (int i = 0; i < 4; ++i) {
//		krnl(i) = i + 1;
//	}
//	for (int i = 0; i < 9; ++i) {
//		img(i) = i + 1;
//	}
//	out.zero();
//	BC::CPU::x_correlation(out.data(), krnl.data(), img.data(), krnl.rank(), krnl.innerShape(), img.size(), img.rows());
//	krnl.print();
//	img.print();
//	out.print();
//	BC::CPU::x_correlation_in(out2.data(), krnl.data(), img.data(), krnl.rank(), krnl.innerShape(), img.size(), img.rows());
//	out2.print();
//	out2.getScalar(1).print();
//	out2(1, 1).print();
//
//

//	x_corr(w, img);

//	speedTestDelayedEval<128,     100000>();
//	speedTestDelayedEval<256,     100000>();
//	speedTestDelayedEval<512,     100000>();
//	speedTestDelayedEval<1024,    100000>();
//	speedTestDelayedEval<2048,    100000>();
//	speedTestDelayedEval<5096,    100000>();
//	speedTestDelayedEval<10000,   100000>();
//	speedTestDelayedEval<20000,   100000>();
//	speedTestDelayedEval<40000,   100000>();
//	speedTestDelayedEval<80000,   100000>();
//	speedTestDelayedEval<100000,  100000>();
//
//	speedTestsScalarIteration<128, 100000>();
//	speedTestsScalarIteration<256, 100000>();
//	speedTestsScalarIteration<512, 100000>();
//	speedTestsScalarIteration<1024, 100000>();
//	speedTestsScalarIteration<2048, 100000>();
//	speedTestsScalarIteration<5096, 100000>();
//	speedTestsScalarIteration<10000, 100000>();
//	speedTestsScalarIteration<20000, 100000>();
//	speedTestsScalarIteration<40000, 100000>();
//	speedTestsScalarIteration<80000, 100000>();
//	speedTestsScalarIteration<100000, 100000>();
////	MatrixPointwise();
	test();

	std::cout << " success  main" << std::endl;

	return 0;
}
