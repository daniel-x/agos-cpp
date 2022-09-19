#ifndef AGOS_COMPILER_COMPILER_H_
#define AGOS_COMPILER_COMPILER_H_

#include <cstdint>
#include "../machine/Machine.h"
#include "ExecutableMemory.h"

using namespace std;

namespace agos {

void printHex(void const* begin, void const* end);

class Compiler {
public:
	typedef void (NativeExecutionFunc)(Machine* state, size_t stepLimit);

	static Compiler* getLocalInstance();

	virtual void compileMachine(const Machine& src, ExecutableMemory& dst) = 0;

	virtual uint8_t* compileInstr(const Machine& src, size_t instrIdx,
			ExecutableMemory& dst) = 0;

protected:
	Compiler() {
	}
	virtual ~Compiler() {
	}

private:
	Compiler(Compiler const&) = delete;
	void operator=(Compiler const&) = delete;
};

} /* namespace agos */

#endif /* AGOS_COMPILER_COMPILER_H_ */
