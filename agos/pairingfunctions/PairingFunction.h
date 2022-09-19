#include <limits>
#include <numeric>
#include <algorithm>

#ifndef PAIRINGFUNCTIONS_PAIRINGFUNCTION_H_
#define PAIRINGFUNCTIONS_PAIRINGFUNCTION_H_

namespace agos {

class PairingFunction {
public:
	virtual ~PairingFunction();

	virtual void incr(size_t* tuple, size_t elementCount) const = 0;

	virtual size_t encode(const size_t* tuple, size_t elementCount) const = 0;

	virtual size_t* decode(size_t n, size_t elementCount,
			size_t* resultTuple) const = 0;

	static size_t max(const size_t* tuple, size_t elementCount) {
		return *std::max_element(tuple, tuple + elementCount);
	}

	static size_t sum(const size_t* tuple, size_t elementCount) {
		return std::accumulate(tuple, tuple + elementCount, 0);
	}
};

} /* namespace agos */

#endif /* PAIRINGFUNCTIONS_PAIRINGFUNCTION_H_ */
