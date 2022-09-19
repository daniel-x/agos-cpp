#ifndef MACHINE_H_
#define MACHINE_H_

#include <stddef.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "Instruction.h"
#include "Operation.h"
#include "Program.h"
#include "../compiler/ExecutableMemory.h"
#include "../MemoryBlock.h"

namespace agos {

class OperationGoedeler;

typedef uint64_t register_type;

class Machine {
public:
	class Layout {
	public:
		size_t instrCount;
		size_t regstCount;
	};

	class State {
	public:
		class Phase {
		public:
			int64_t code;
			Phase(Phase const& other) {
				code = other.code;
			}
			Phase(int64_t code) {
				this->code = code;
			}
			bool operator==(Phase const& other) const {
				return code == other.code;
			}
			bool operator!=(Phase const& other) const {
				return code != other.code;
			}
			static const Phase RUNNABLE;
			static const Phase FINISHED;
			static const Phase ABORTED_DIVZERO;
			static const Phase ABORTED_INFINITELOOP;
			std::string toString() const;
		};

		int64_t instrPtr;

		Phase phase;

		MemoryBlock memory;

		State(size_t regstCount = 1);

		bool operator==(State const& other) const;
		bool operator!=(State const& other) const;

		void reset();

		std::string toString() const;
	private:
		void regstAlloc(size_t regstCount);
		void regstRealloc(size_t regstCount);
		void regstDealloc();
		size_t getRegstCount() const;
		void setRegstCount(size_t regstCount);

		friend Machine;
	};

protected:
	size_t stepCount;
	State state;

	size_t nextStoreStep;
	State storedState;

	Program program;

	OperationGoedeler& opGoed;

	mutable ExecutableMemory nativeMachine;

public:
	Machine();
	Machine(size_t regstCount);
	Machine(size_t instrCount, size_t regstCount);
	Machine(const Program& program, size_t regstCount);

	Program& getProgram();
	Program const& getProgram() const;
	size_t getRegstCount() const;
	size_t getInstrCount() const;
	Instruction getInstrZero() const;
	int64_t getRegst(size_t idx) const;
	State::Phase getPhase() const;

	State getState() const;

	// BigInteger getInstrSpaceSize() const;
	size_t getParamSpaceSize(Operation::ParamType paramType) const;
	// BigInteger getInstrSpaceSizeForOp(Operation op) const;

	bool containsKnownUselessInstruction() const;
	bool isKnownUseless(Instruction const& instr, size_t idx) const;

	void setInstrCount(size_t count);
	void setRegstCount(size_t count);
	void setRegst(size_t idx, int64_t value);

	void reset();

	/**
	 * When this function returns, one of 3 cases happened:
	 * 1. The machine finished execution. This happens either by
	 * executing an hlt instruction or by executing the last
	 * instruction, which is no jump. The machine's phase will
	 * be FINISHED and the instrPtr will point one behind the
	 * last executed instruction.
	 * 2. An instruction couldn't be executed, e.g. because of
	 * div0. Then the phase will resemble the error that occurred
	 * and the instrPtr will point to the instruction which could
	 * not be executed.
	 * 3. The step limit was reached before the machine succeeded
	 * or failed running. Then the phase is RUNNING and instrPtr
	 * will point to the next instruction to be executed when the
	 * running is continued.
	 */
	void run(int64_t stepLimit);

	void compile() const;
	void compileInstr(size_t instrIdx) const;
	void runNatively(int64_t stepLimit);

	std::string toStringWithState() const;
	std::string toStringBasic() const;
	std::string toString() const;
	std::string toStringMultiline() const;
};

}

#endif /* MACHINE_H_ */
