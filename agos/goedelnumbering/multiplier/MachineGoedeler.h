#include "../../machine/Machine.h"
#include "../../pairingfunctions/CantorPairingFunction.h"
#include "ProgramGoedeler.h"

#ifndef AGOS_GOEDELNUMBERING_MULTIPLIER_MACHINEGOEDELER_H_
#define AGOS_GOEDELNUMBERING_MULTIPLIER_MACHINEGOEDELER_H_

namespace agos {

class MachineGoedeler {
private:
	MachineGoedeler(MachineGoedeler const&) = delete;
	void operator=(MachineGoedeler const&) = delete;

	ProgramGoedeler& progGoed;
	CantorPairingFunction& pairingFunc;

	MachineGoedeler() :
			progGoed(ProgramGoedeler::getInstance()), //
			pairingFunc(CantorPairingFunction::getInstance()) {
	}

public:
	static MachineGoedeler& getInstance() {
		static MachineGoedeler INSTANCE;
		return INSTANCE;
	}

	bool incr(Machine& m, size_t* modifiedInstrCount = NULL) const;

	bool incrToNoKnownUselessInstr(Machine& m) const;

//	BigInteger encode(Machine const& machine) const;
//	Machine decode(BigInteger const& goedelNumber) const;
};

} /* namespace agos */

#endif /* AGOS_GOEDELNUMBERING_MULTIPLIER_MACHINEGOEDELER_H_ */
