#include <cstddef>
#include <cstdint>
#include "PairingFunction.h"

#ifndef PAIRINGFUNCTIONS_CANTORTUPLEINCREMENTER_H_
#define PAIRINGFUNCTIONS_CANTORTUPLEINCREMENTER_H_

namespace agos {

class CantorPairingFunction: public PairingFunction {
private:
	CantorPairingFunction() {
	}
	CantorPairingFunction(CantorPairingFunction const&) = delete;
	void operator=(CantorPairingFunction const&) = delete;

public:
	static CantorPairingFunction& getInstance() {
		static CantorPairingFunction INSTANCE;
		return INSTANCE;
	}

	void incr(size_t* t, size_t elementCount) const;
	size_t encode(const size_t* t, size_t elementCount) const;
	size_t* decode(size_t n, size_t elementCount, size_t* result) const;

	static void test();
};

} /* namespace agos */

#endif /* PAIRINGFUNCTIONS_CANTORTUPLEINCREMENTER_H_ */
