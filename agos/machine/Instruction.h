#include <cstdint>
#include <string>

#include "Operation.h"

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

namespace agos {

class Instruction {
public:
	Operation op;

	size_t param0;

	Instruction();
	Instruction(Operation op);
	Instruction(Operation op, size_t param0);
	Instruction(Operation op, size_t param0, size_t param1);
	Instruction(Instruction const& src);
	Instruction(Operation op, size_t* params);
	Instruction& operator=(Instruction const& src);
	bool operator==(Instruction const& other) const;
	Operation& getOp();
	Operation getOp() const;
	void setOp(Operation op);
	size_t getParamCount() const;
	Operation::ParamType getParamType(size_t idx) const;

	size_t getParam(size_t idx) const;
	void setParam(size_t idx, size_t value);
	std::string toString() const;
	std::string paramToString(size_t idx) const;
	std::string param0ToString() const;
	static std::string paramToString(size_t param,
			Operation::ParamType paramType);
	static std::string toIdentifierName(size_t i);
	size_t getParam0() const;
	void resetParams();

private:
	void couldntHandleParamIdx(size_t idx) const;
};

}

#endif /* INSTRUCTION_H_ */
