//#ifndef DEP
//#define DEP
//namespace BC {
////template<class T, class lv, class rv, class Mathlib>
//struct dp_impl {
//	template<class class_> using TRANS = unary_expression_transpose<_scalar<class_>, class_>;
//	template<class U, class l1, class l2> using BESR_mul = binary_expression_scalar_R<U, mul, l1, l2>;
//	template<class U, class l1, class l2> using BESL_mul = binary_expression_scalar_L<U, mul, l1, l2>;
//	template<class U, class l1, class l2> using BESR_trans_mul = binary_expression_scalar_R<U, mul, TRANS<l1>, l2>;
//	template<class U, class l1, class l2> using BESL_trans_mul = binary_expression_scalar_L<U, mul, l1, TRANS<l2>>;
//
//
//	template<class U> __BCinline__ static constexpr std::true_type isTransposed(const TRANS<U>&) { return std::true_type(); }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr std::true_type isTransposed(const BESR_trans_mul<U, lv, rv>&) { return std::true_type(); }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr std::true_type isTransposed(const BESL_trans_mul<U, lv, rv>&) { return std::true_type(); }
//	template<class U> __BCinline__ static constexpr std::false_type isTransposed(const U&) { return std::false_type(); }
//
//	template<class U, class lv, class rv>  __BCinline__ static constexpr std::true_type hasScalar(const BESR_trans_mul<U, lv, rv>&) { return std::true_type(); }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr std::true_type hasScalar(const BESL_trans_mul<U, lv, rv>&) { return std::true_type(); }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr std::true_type hasScalar(const BESR_mul<U, lv, rv>&) { return std::true_type(); }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr std::true_type hasScalar(const BESL_mul<U, lv, rv>&) { return std::true_type(); }
//	template<class U> __BCinline__ static constexpr std::false_type hasScalar(const U&) { return std::false_type(); }
//
//	template<class U> __BCinline__ static constexpr const auto getArray(const TRANS<U>& param) { return param.array; }
//	template<class U> __BCinline__ static constexpr const auto getArray(const U& param) { return param; }
//	template<class U, class lv, class rv>  __BCinline__  static constexpr const auto getArray(const BESR_trans_mul<U, lv, rv>& param) { return param.left; }
//	template<class U, class lv, class rv>  __BCinline__  static constexpr const auto getArray(const BESL_trans_mul<U, lv, rv>& param) { return param.right; }
//
//	template<class U> __BCinline__ static constexpr const auto& getScalar(const U& param) { return param; }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr const auto getScalar(const BESR_trans_mul<U, lv, rv>& param) { return param.right; }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr const auto getScalar(const BESL_trans_mul<U, lv, rv>& param) { return param.left; }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr const auto getScalar(const BESR_mul<U, lv, rv>& param) { return param.right; }
//	template<class U, class lv, class rv>  __BCinline__ static constexpr const auto getScalar(const BESL_mul<U, lv, rv>& param) { return param.left; }
//};
//
//template<class T> struct isTrue { static constexpr bool value = false; };
//template<> struct isTrue<std::true_type> { static constexpr bool value = true; };
//
//template<class T, class lv, class rv, class Mathlib>
//struct binary_expression_dotproduct : expression<T, binary_expression_dotproduct<T, lv, rv, Mathlib>> {
//	using scalar_type = typename MTF::determine_scalar<T>::type;
//	using impl = dp_impl;
//
//	static constexpr bool transA = isTrue<decltype(impl::isTransposed(std::declval<lv>()))>::value; //det_eval<lv>::transposed;
//	static constexpr bool transB = isTrue<decltype(impl::isTransposed(std::declval<rv>()))>::value;
//	static constexpr bool lv_scalar = isTrue<decltype(impl::hasScalar(std::declval<lv>()))>::value;
//	static constexpr bool rv_scalar = isTrue<decltype(impl::hasScalar(std::declval<rv>()))>::value;
//
//	lv left;
//	rv right;
//
//	decltype(impl::getArray(std::declval<lv>())) A;    //= impl::getArray(left);
//	decltype(impl::getScalar((std::declval<lv>()))) scalA; //= impl::getScalar(left);
//	decltype(impl::getArray((std::declval<rv>()))) B; //= impl::getArray(right);
//	decltype(impl::getScalar((std::declval<rv>()))) scalB;// = impl::getScalar(right);
//
//	__BCinline__ binary_expression_dotproduct(lv left, rv right) : left(left), right(right),
//	A(impl::getArray(left)), B(impl::getArray(right)),
//	  scalA(impl::getScalar(left)), scalB(impl::getScalar(right))
//	{}
//
//	__BCinline__ int M()   const { return left.rows(); 		}
//	__BCinline__ int N()   const { return right.cols(); 	}
//	__BCinline__ int K()   const { return left.cols(); 		}
//	__BCinline__ int LDA() const { return left.LD_rows(); 	}
//	__BCinline__ int LDB() const { return right.LD_rows(); }
//	__BCinline__ int LDC() const { return M(); }
//
//	__BCinline__ int size() const { return M() * N();}
//	__BCinline__ int rows() const { return M();}
//	__BCinline__ int cols() const { return N();}
//	__BCinline__ int rank() const { return right.rank(); }
//	__BCinline__ int LD_rows() const { return M(); }
//	__BCinline__ int LD_cols() const { return size(); }
//	__BCinline__ int dimension(int i)		const { return i== 0 ? M() : i == 1 ? N() : 1; }
//	__BCinline__ const auto innerShape() 	const { return generateDimList(M(), N()); }
//	__BCinline__ const auto outerShape() 	const { return generateDimList(M(), size()); }
//
//	void printDimensions() 		const { std::cout<<"[" << M() << "][" << N()  <<"]" << std::endl; }
//	void printLDDimensions()	const { std::cout<<"[" << M() << "][" << size()  <<"]" << std::endl; }
//
//	__BCinline__ T operator [](int index) const {
//		T sum = 0;
//
//		int COL = (int) (index / M());
//		int ROW = (int) (index % M());
//
//		if (!transA) {
//			if (!transB) {
//				int COL_BASE = COL * right.LD_rows();
//
//				for (int i = 0; i < left.cols(); ++i) {
//					sum += A[ROW + i * A.LD_rows()] * B[COL_BASE + i];
//				}
//
//			} else /* transB */ {
//				for (int i = 0; i < left.cols(); ++i) {
//					sum += A[ROW + i * A.LD_rows()] * B[COL + i * B.LD_rows()];
//				}
//			}
//		} else {
//			int ROW_BASE = ROW * left.LD_rows();
//
//			if (!transB) {
//				int COL_BASE = COL * right.LD_rows();
//
//				for (int i = 0; i < left.cols(); ++i) {
//					sum += A[ROW_BASE + i] * B[COL_BASE + i];
//				}
//			} else /* transB */ {
//				for (int i = 0; i < left.cols(); ++i) {
//					sum += A[ROW_BASE + i] * B[COL + i * B.LD_rows()];
//				}
//			}
//
//		}
//
//		if (lv_scalar)
//			if (rv_scalar)
//				return sum * scalA[0] * scalB[0];
//			else
//				return sum * scalA[0];
//		else
//			if (rv_scalar)
//				return sum * scalB[0];
//			else
//				return sum;
//	}
//
//
//
//
//	};
//}
//
//#endif /* EXPRESSION_BINARY_DOTPRODUCT_CU_ */
//
//
//
////				Some printouts for debugging
////
////		std::cout << "dotproduct stats --------------------------------------------------------------------------" << std::endl;
////				std::cout << " m n k = " << M << "  "<< N << " " << K << std::endl;
////
////				if (lv_needs_to_be_evaluated) {
////					if (self_eval<lv>::conditioanl) {
////						std::cout << " lv self eval " << std::endl;
////					}
////					std::cout << " lv was evaluated " << std::endl;
////				}
////				if (rv_needs_to_be_evaluated) {
////					if (self_eval<rv>::conditioanl) {
////						std::cout << " rv self eval " << std::endl;
////					}
////					std::cout << " lv was evaluated " << std::endl;
////				}
////				if (transA) {
////					std::cout << " A - fast trans " << std::endl;
////				}
////				if (transB) {
////					std::cout << " B - fast trans " << std::endl;
////				}
////				if (evaluate<lv>::scalar) {
////					std::cout << " lv scalar detected " << std::endl;
////				}
////				if (evaluate<rv>::scalar) {
////					std::cout << " rv scalar detected " << std::endl;
////				}
////				if (scal_A && scal_B)
////				std::cout << "scalars = " << *scal_A <<  " " << *scal_B << std::endl;
////				std::cout << " --------------------------------------------------------------------------" << std::endl;
//
//