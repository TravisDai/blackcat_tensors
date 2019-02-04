/*
 * Common.h
 *
 *  Created on: Jan 13, 2019
 *      Author: joseph
 */

#ifndef BC_STREAMS_COMMON_H_
#define BC_STREAMS_COMMON_H_

#include <memory>
#include <iostream>
#include "HostQueue.h"
namespace BC {
namespace context {

template<class Queue>
class Stream {

	std::shared_ptr<Queue> m_job_queue;

public:

	Stream() : m_job_queue(nullptr) {}
	Stream(const Stream&)=default;
	Stream(Stream&&)=default;

	bool is_default_stream() {
		return bool(m_job_queue.get());
	}

	void create_stream() {
		m_job_queue = std::shared_ptr<Queue>(new Queue());
		m_job_queue.get()->init();
	}

	void delete_stream() {
		m_job_queue = std::shared_ptr<Queue>(nullptr);
	}

	void sync_stream() {
		//** Pushing a job while syncing is undefined behavior.
		m_job_queue.get()->synchronize();
	}

	void set_stream(Stream<Queue>& stream_) {
		this->m_job_queue = stream_.m_job_queue;
	}

	template<class function_lambda>
	void push_job(function_lambda functor) {
		if (!m_job_queue.get()) {
			functor();
		} else {
			m_job_queue.get()->push(functor);
		}
	}
};
}
}


#endif /* COMMON_H_ */
