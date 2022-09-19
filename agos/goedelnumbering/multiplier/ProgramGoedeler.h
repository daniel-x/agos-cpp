#include "InstructionGoedeler.h"
#include "../../machine/Program.h"

#ifndef AGOS_GOEDELNUMBERING_MULTIPLIER_PROGRAMGOEDELER_H_
#define AGOS_GOEDELNUMBERING_MULTIPLIER_PROGRAMGOEDELER_H_

namespace agos {

class ProgramGoedeler {
private:
	ProgramGoedeler(ProgramGoedeler const&) = delete;
	void operator=(ProgramGoedeler const&) = delete;

	InstructionGoedeler& instrGoed;

	ProgramGoedeler() :
			instrGoed(InstructionGoedeler::getInstance()) {
	}

public:
	static ProgramGoedeler& getInstance() {
		static ProgramGoedeler INSTANCE;
		return INSTANCE;
	}

	bool incr(Program& program, Machine const& machine,
			size_t* modifiedInstrCount = NULL);

//	BigInteger encode(Program const& prog, Machine const& machine) const;
//	Program decode(BigInteger const& goedelNumber,
//			Machine const& machine) const;
};

} /* namespace agos */

#endif /* AGOS_GOEDELNUMBERING_MULTIPLIER_PROGRAMGOEDELER_H_ */
