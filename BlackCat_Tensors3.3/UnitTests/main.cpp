#include <iostream>
#include "../BlackCat_Tensors.h"
#include <typeinfo>
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
using tensor4 = BC::Tensor4<float, ml>;
using tesnor5 = BC::Tensor5<float, ml>;


//std::vector<unsigned int> internal_type;
//using ary = std::vector<unsigned int>;

#include "_correlation_test.h"
#include "_dotproducts_test.h"
#include "_dotproduct_injection_test.h"

#include "_readwrite_test.h"
#include "_shaping_test.h"
#include <iostream>
//#include "_speed_benchmark.h"
//#include "_d1_xcorr_test.h"

#include <iostream>


#include <cxxabi.h>

std::string removeNS( const std::string & source, const std::string & namespace_ )
{
    std::string dst = source;
    size_t position = source.find( namespace_ );
    while ( position != std::string::npos )
    {
        dst.erase( position, namespace_.length() );
        position = dst.find( namespace_, position + 1 );
    }
    return dst;
}

template<class T>
std::string type_name() {
	int status;
	  std::string demangled = abi::__cxa_demangle(typeid(T).name(),0,0,&status);
	  return removeNS(removeNS(removeNS(demangled, "BC::"), "internal::"), "function::");
}

int main() {

	//various tests
//	correlation();
//	dotproducts();
	dotproduct_injection();
//	readwrite();
//	shaping();


	std::cout << " success  main" << std::endl;

}
