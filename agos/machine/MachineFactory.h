#include "Machine.h"

#ifndef MACHINE_MACHINEFACTORY_H_
#define MACHINE_MACHINEFACTORY_H_

namespace agos {

class MachineFactory {
private:
	MachineFactory() = delete;
	MachineFactory(MachineFactory const&) = delete;
	void operator=(MachineFactory const&) = delete;
public:
	static Machine createIsNotMultipleOf3();
	static Machine createTimes5Plus1();
	static Machine createTimes2();
	static Machine createPlus1();
	static Machine createPlus5();
};

} /* namespace agos */

#endif /* MACHINE_MACHINEFACTORY_H_ */
