#include <cstdint>
#include <string>
#include <sstream>

#ifndef STRINGFORMAT_H_
#define STRINGFORMAT_H_

namespace agos {

std::string strfmt(const std::string fmt, ...);

template<class InputIterator>
std::string toString(InputIterator first, InputIterator last) {
	std::ostringstream oss;

	if (last != first) {
		for (; first + 1 != last; ++first) {
			oss << *first << ' ';
		}

		oss << *first;
	}

	return oss.str();
}

template<class InputIterator>
std::string toString(InputIterator first, size_t count) {
	return toString(first, first + count);
}

//std::string toString(BigInteger i);

std::string toStringWithSuffix(size_t v);
//std::string toStringWithSuffix(BigInteger i);

} /* namespace agos */

#endif /* STRINGFORMAT_H_ */
