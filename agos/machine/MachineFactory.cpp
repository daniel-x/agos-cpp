#include "MachineFactory.h"

namespace agos {

Machine MachineFactory::createPlus1() {
	Machine m;

	Program& p = m.getProgram();
	p.push_back(Instruction(Operation::inc));

	return m;
}

Machine MachineFactory::createPlus5() {
	Machine m;

	Program& p = m.getProgram();
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));

	return m;
}

Machine MachineFactory::createTimes2() {
	Machine m(2);

	Program& p = m.getProgram();
	p.push_back(Instruction(Operation::swp, 1));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::mul, 1));

	return m;
}

Machine MachineFactory::createTimes5Plus1() {
	Machine m(2);

	Program& p = m.getProgram();
	p.push_back(Instruction(Operation::swp, 1));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::add, (size_t) 0));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::mul, 1));
	p.push_back(Instruction(Operation::inc));

	return m;
}

Machine MachineFactory::createIsNotMultipleOf3() {
	Machine m(2);

	Program& p = m.getProgram();
	p.push_back(Instruction(Operation::swp, 1));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::inc));
	p.push_back(Instruction(Operation::swp, 1));
	p.push_back(Instruction(Operation::mul, (size_t) 0));
	p.push_back(Instruction(Operation::mod, 1));

	return m;
}

} /* namespace agos */
