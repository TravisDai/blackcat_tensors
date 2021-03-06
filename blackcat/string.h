/*
 * string_extensions.h
 *
 *  Created on: Nov 11, 2019
 *      Author: joseph
 */

#ifndef BLACKCAT_TENSORS_STRING_EXTENSIONS_H_
#define BLACKCAT_TENSORS_STRING_EXTENSIONS_H_

#include <string>
#include <vector>
#include <algorithm>

namespace bc {

/**
 * Inherits from std::string.
 * Simply defines additional "extensions" to std::string.
 */
struct string:
		std::string {

	using std::string::string;
	string() = default;
	string(const string&) = default;
	string(string&&) = default; 
	string& operator =(const string&) = default;
	string& operator =(string&&) = default; 

	string(std::string s):
		std::string(s) {}

	int count(char value) const {
		return std::count(this->begin(), this->end(), value);
	}

	bool startswith(const std::string& str) const {
		return this->size() >= str.size() &&
				this->substr(0, str.size()) == str;
	}

	bool endswith(const bc::string& str) const {
		if (this->size() < str.size())
			return false;

		auto start_idx = this->size() - str.size();
		return this->substr(start_idx, str.size()) == str;
	}

	bool startswith(char c) const {
		return this->size() && (*this)[0] == c;
	}

	bool endswith(char c) const {
		return this->size() && this->back() == c;
	}


	std::vector<bc::string> split(char delim) const {
		std::vector<bc::string> splits;

		auto curr = this->begin();
		auto end = this->end();

		while (curr < end) {
			auto next = std::find(curr, end, delim);

			if (curr < end)
				splits.push_back(bc::string(curr, next));

			curr = next + 1;
		}

		return splits;
	}
};


}



#endif /* STRING_EXTENSIONS_H_ */
