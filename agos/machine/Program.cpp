#include "Program.h"

#include <cstdint>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <utility>

#include "Operation.h"

namespace agos {

using std::ostringstream;
using std::set;
using std::string;

Program::Program() {
}

Program::Program(size_t size) :
		vector(size, Instruction()) {
}

void Program::resetInstructions() {
	std::fill(begin(), end(), Instruction());
}

string Program::toStringMultiline() const {
	ostringstream oss;

	// declare register vars
	oss << "int64_t run(int64_t& a) {\n";

	set<size_t> usedRegisters;
	set<size_t> usedInstrIdxs;

	bool otherUsedRegisters = false;
	// registers as vars
	for (const_iterator it = begin(); it < end(); ++it) {
		Instruction const& instr = *it;

		for (size_t i = 0; i < instr.getParamCount(); i++) {
			Operation::ParamType paramType = instr.getParamType(i);
			size_t param = instr.getParam(i);

			if (paramType == Operation::INSTRUCTION_INDEX) {
				usedInstrIdxs.insert(param);
			} else if (paramType == Operation::REGISTER_INDEX) {
				if (usedRegisters.insert(param).second && param != 0) {
					otherUsedRegisters = true;
					oss << "    int64_t ";
					oss << instr.paramToString(param, paramType);
					oss << " = 0;\n";
				}
			}
		}
	}

	if (otherUsedRegisters) {
		oss << "\n";
	}

	// list instructions
	size_t idx = 0;
	for (const_iterator it = begin(); it < end(); ++it) {
		Instruction const& instr = *it;

		if (usedInstrIdxs.find(idx) != usedInstrIdxs.end()) {
			oss << "    ";
			oss
					<< Instruction::paramToString(idx,
							Operation::INSTRUCTION_INDEX);
			oss << ":\n";
		}

		oss << "    " << instr.toString() << '\n';

		idx++;
	}
	oss << "}";

	return oss.str();
}

string Program::toString() const {
	ostringstream oss;

	oss << '[';

	if (!empty()) {
		for (const_iterator it = begin(); it < end() - 1; ++it) {
			Instruction const& instr = *it;
			oss << instr.toString() << ' ';
		}
		oss << back().toString();
	}

	oss << ']';

	return oss.str();
}

}
