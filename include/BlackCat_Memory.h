/*
 * BlackCat_Memory.h
 *
 *  Created on: May 14, 2019
 *      Author: joseph
 */

#ifndef BLACKCAT_MEMORY_H_
#define BLACKCAT_MEMORY_H_

#include "BlackCat_Allocator.h"
#include <memory>
#include <mutex>

namespace BC {
namespace memory {
namespace detail {

struct Default_Deleter {
	template<class T>
	void operator ()(T* ptr) {
		delete ptr;
	}
} default_deleter;


}


template<class ValueType>
class atomic_shared_ptr {

	mutable std::mutex locker;
	std::shared_ptr<ValueType> m_ptr;

	template<class... Args>
	atomic_shared_ptr(Args&&... args) : m_ptr(args...) {};
	template<class... Args>
	atomic_shared_ptr(const Args&... args) : m_ptr(args...) {};

	struct wrapper {
		std::mutex& locker;
		std::shared_ptr<ValueType> m_ptr;
		~wrapper() {
			locker.unlock();
		}

		auto operator * () const  { return m_ptr->get(); }
		auto operator * () { return m_ptr->get(); }

		auto operator ->() const { return m_ptr->get(); }
		auto operator ->()  { return m_ptr->get(); }
	};


	template<class Y, class Deleter=detail::Default_Deleter, class Allocator=std::allocator<ValueType>>
	void reset(Y* ptr, Deleter del=detail::default_deleter, Allocator alloc=Allocator()) {
		locker.lock();
		m_ptr.reset(ptr, del, alloc);
		locker.unlock();
	}
	void swap(std::shared_ptr<ValueType>& sh_ptr) {
		locker.lock();

		locker.unlock();
	}

	wrapper get() {
		locker.lock();
		return wrapper {locker, m_ptr};
	}
	const wrapper get() const {
		locker.lock();
		return wrapper {locker, m_ptr};
	}

	auto operator ->() { return this->get(); }
	auto operator ->() const { return this->get(); }

};

using namespace memory;
}
}
#endif /* BLACKCAT_MEMORY_H_ */
