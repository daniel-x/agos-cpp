#include "MachineGoedeler.h"
#include "../../machine/Program.h"

namespace agos {

bool MachineGoedeler::incr(Machine& m, size_t* modifiedInstrCount) const {
	bool carry = progGoed.incr(m.getProgram(), m, modifiedInstrCount);

	if (carry) {
		size_t instrCount = m.getInstrCount();
		size_t regstCount = m.getRegstCount();

		size_t t[2];
		t[0] = instrCount;
		t[1] = regstCount - 1;

		pairingFunc.incr(t, 2);

		instrCount = t[0];
		regstCount = t[1] + 1;

		m.setInstrCount(instrCount);
		m.setRegstCount(regstCount);

		m.getProgram().resetInstructions();

		if (modifiedInstrCount != NULL) {
			*modifiedInstrCount = m.getInstrCount();
		}
	}

	return carry;
}

bool MachineGoedeler::incrToNoKnownUselessInstr(Machine& m) const {
	InstructionGoedeler& instrGoed = InstructionGoedeler::getInstance();

	Program& prog = m.getProgram();

	size_t instrIdx = 0;
	bool carry = false;
	for (Program::iterator it = prog.begin(); it < prog.end(); ++it) {
		if (carry) {
			carry = instrGoed.incr(*it, m);
		}

		while (m.isKnownUseless(*it, instrIdx)) {
			carry |= instrGoed.incr(*it, m);
		}

		instrIdx++;
	}

	return carry;
}

//BigInteger MachineGoedeler::encode(Machine const& machine) const {
//	BigInteger result = 0;
//
//	BigInteger progGoedNo = progGoed.encode(machine.getProgram(), machine);
//
//	result += progGoedNo;
//
//	return result;
//}

//Machine MachineGoedeler::decode(BigInteger const& goedelNumber) const {
//	return Machine();
//}

} /* namespace agos */
