#include "OperationGoedeler.h"

namespace agos {

bool OperationGoedeler::incr(Operation& op, Machine const& machine) const {
	++op;

	if (op > Operation::LAST) {
		op = Operation::FIRST;
		return true;
	} else {
		return false;
	}
}

} /* namespace agos */
