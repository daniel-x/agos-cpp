#ifndef AGOS_PROTOTYPEFUNCS_X64_H_
#define AGOS_PROTOTYPEFUNCS_X64_H_

#include <cstdint>
#include <cstdlib>

namespace agos {

#define PLACEHOLDER_Q_BARE 0x1122334455667788
#define PLACEHOLDER_D_BARE 0x44332211
#define XSTR(s) STR(s)
#define STR(s) #s
#define PLACEHOLDER_Q ((uint64_t)PLACEHOLDER_Q_BARE) // 88 77 66 55 44 33 22 11
#define PLACEHOLDER_D ((uint32_t)PLACEHOLDER_D_BARE) // 11 22 33 44

class PrototypeFuncs_x64 {
public:
	typedef uint8_t* (PrototypeFunc)();

	typedef enum {
		INSTR_HLT, // instructions first and they have to be in the same order as the opcodes

		INSTR_SWP, //

		INSTR_INC, //
		INSTR_DEC, //

		INSTR_ADD, //
		INSTR_SUB, //
		INSTR_MUL, //
		INSTR_DIV, //
		INSTR_MOD, //

		INSTR_JMZ, //

		MACHINE_PROL, //
		MACHINE_EPIL, //
		INSTR_PROL, //
		LAST_INSTR_EPIL, //
		COUNT // hack for number of elements
	} Id;

	class Info {
	public:
		PrototypeFunc* funcPtr;

		uint8_t* begin;
		size_t size;

		static const size_t PLACEHOLDER_OFFSET_COUNT_MAX = 8;
		int16_t placeholderOffset[PLACEHOLDER_OFFSET_COUNT_MAX];

		char const* name;

		void init(PrototypeFunc* funcPtr, char const* name);

		void initPlaceholderOffsets();
	};

	static Info LIST[];

	static PrototypeFuncs_x64* getInstance();

	static void initList();

	static uint8_t* getCodeBegin(PrototypeFuncs_x64::PrototypeFunc* protoFunc);
	static uint8_t* getCodeEnd(PrototypeFuncs_x64::PrototypeFunc* protoFunc);

	static uint8_t* findPlaceholderAddr(uint8_t* begin, uint8_t* end);

	static void initLeaRipRaxCodeSize();
	static void initStandardFuncPrologueCodeSize();
	static void initInstructionCodeSize();

	static void printDetails();

	static uint8_t* instrInc();
	static uint8_t* instrDec();
	static uint8_t* instrSwp();
	static uint8_t* instrAdd();
	static uint8_t* instrSub();
	static uint8_t* instrDiv();
	static uint8_t* instrMul();
	static uint8_t* instrMod();
	static uint8_t* instrJmz();
	static uint8_t* instrHlt();
	static uint8_t* machineProl();
	static uint8_t* machineEpil();
	static uint8_t* instrProl();
	static uint8_t* lastInstrEpil();

	static size_t leaRipRaxCodeSize;
	static size_t standardFuncPrologueCodeSize;
	static size_t instructionCodeSize;
	static size_t specificInstrMaxCodeSize;

private:
	PrototypeFuncs_x64(PrototypeFuncs_x64 const&) = delete;
	void operator=(PrototypeFuncs_x64 const&) = delete;

	PrototypeFuncs_x64();
};

} /* namespace agos */

#endif /* AGOS_PROTOTYPEFUNCS_X64_H_ */
