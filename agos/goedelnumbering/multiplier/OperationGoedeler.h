#ifndef AGOS_GOEDELNUMBERING_MULTIPLIER_OPERATIONGOEDELER_H_
#define AGOS_GOEDELNUMBERING_MULTIPLIER_OPERATIONGOEDELER_H_

#include "../../machine/Machine.h"
#include "../../machine/Operation.h"

namespace agos {

class OperationGoedeler {
private:
	OperationGoedeler(OperationGoedeler const&) = delete;
	void operator=(OperationGoedeler const&) = delete;

	OperationGoedeler() {
	}

public:
	static OperationGoedeler& getInstance() {
		static OperationGoedeler INSTANCE;
		return INSTANCE;
	}

	bool incr(Operation& p, Machine const& machine) const;
};

} /* namespace agos */

#endif /* AGOS_GOEDELNUMBERING_MULTIPLIER_OPERATIONGOEDELER_H_ */
