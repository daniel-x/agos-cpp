#include <string>
#include <vector>

#include "Instruction.h"

#ifndef PROGRAM_H_
#define PROGRAM_H_

namespace agos {

class Program: public std::vector<Instruction> {
public:
	Program();
	Program(size_t size);

	void resetInstructions();
	std::string toStringMultiline() const;
	std::string toString() const;
};

}

#endif /* PROGRAM_H_ */
