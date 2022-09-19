#include "../../machine/Operation.h"
#include "../../machine/Machine.h"
#include "OperationGoedeler.h"

#ifndef GOEDELNUMBERING_MULTIPLIER_INSTRUCTIONGOEDEL_H_
#define GOEDELNUMBERING_MULTIPLIER_INSTRUCTIONGOEDEL_H_

namespace agos {

class InstructionGoedeler {
private:
	InstructionGoedeler(InstructionGoedeler const&) = delete;
	void operator=(InstructionGoedeler const&) = delete;

	OperationGoedeler& opIncr;

	InstructionGoedeler() :
			opIncr(OperationGoedeler::getInstance()) {
	}

public:
	static InstructionGoedeler& getInstance() {
		static InstructionGoedeler INSTANCE;
		return INSTANCE;
	}

	bool incr(Instruction& p, const Machine& machine);

//	BigInteger encode(Instruction const& instr, Machine const& machine) const;
//	Instruction decode(BigInteger const& goedelNumber,
//			Machine const& machine) const;
};

} /* namespace agos */

#endif /* GOEDELNUMBERING_MULTIPLIER_INSTRUCTIONGOEDEL_H_ */
