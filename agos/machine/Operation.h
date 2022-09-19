#include <cstddef>
#include <string>

#ifndef OPERATION_H_
#define OPERATION_H_

namespace agos {

class Operation {

public:
	enum ParamType {
		INSTRUCTION_INDEX, //
		REGISTER_INDEX, //
	};

	enum Opcode {
		hlt, // halt

		swp, // swap(register[0], register[a])

		inc, // register[0]++
		dec, // register[0]--

		add, // register[0] += register[a]
		sub, // register[0] -= register[a]
		mul, // register[0] *= register[a]
		div, // register[0] /= register[a]
		mod, // register[0] %= register[a]

		jmz, // if (register[0] == 0) goto a

		FIRST = hlt,
		LAST = jmz,
	};

	static const size_t COUNT = LAST - FIRST + 1; // end - begin: no +1, but with first and last, we need +1.

	Opcode opcode;

	Operation();

	Operation(Opcode opcode);
	Operation(int opcode);

	operator int() const;
	operator Opcode() const;
	Operation& operator++();

	bool operator==(Operation const& other) const;

	size_t getParamCount() const;
	ParamType getParamType(size_t paramIdx) const;

	std::string toString() const;

	void throwMethodNotPreparedForOperationException() const;
	static void throwMethodNotPreparedForParamTypeException(
			ParamType paramType);
};

}

#endif /* OPERATION_H_ */
