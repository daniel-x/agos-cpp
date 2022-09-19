#include "ProgramGoedeler.h"

namespace agos {

bool ProgramGoedeler::incr(Program& program, Machine const& machine,
		size_t* modifiedInstrCount) {
	bool carry = true;

	size_t instrIdx = 0;
	for (Program::iterator it = program.begin(); it != program.end() && carry;
			++it) {
		carry = instrGoed.incr(*it, machine);

		while (machine.isKnownUseless(*it, instrIdx)) {
			carry |= instrGoed.incr(*it, machine);
		}

		instrIdx++;
	}

	if (modifiedInstrCount != NULL) {
		*modifiedInstrCount = instrIdx;
	}

	return carry;
}

//BigInteger ProgramGoedeler::encode(Program const& program,
//		Machine const& machine) const {
//	BigInteger result = 0;
//
//	if (!program.empty()) {
//		BigInteger instrSpaceSize = machine.getInstrSpaceSize();
//
//		Program::const_reverse_iterator it;
//		for (it = program.rbegin(); it != program.rend() - 1; ++it) {
//			result += instrGoed.encode(*it, machine);
//			result *= instrSpaceSize;
//		}
//
//		result += instrGoed.encode(*it, machine);
//	}
//
//	return result;
//}
//
//Program ProgramGoedeler::decode(BigInteger const& goedelNumber,
//		Machine const& machine) const {
//	return Program();
//}

} /* namespace agos */
