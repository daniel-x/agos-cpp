#include <cinttypes>
#include "../../machine/Operation.h"
#include "PrototypeFuncs_x64.h"

namespace agos {

PrototypeFuncs_x64* PrototypeFuncs_x64::getInstance() {
	static PrototypeFuncs_x64 INSTANCE;
	return &INSTANCE;
}

PrototypeFuncs_x64::PrototypeFuncs_x64() {
	initLeaRipRaxCodeSize();
	initStandardFuncPrologueCodeSize();

	initList();

	initInstructionCodeSize();

	printDetails();
}

void PrototypeFuncs_x64::initList() {
	LIST[Id::INSTR_INC].init(&instrInc, "instrInc");
	LIST[Id::INSTR_DEC].init(&instrDec, "instrDec");
	LIST[Id::INSTR_SWP].init(&instrSwp, "instrSwp");
	LIST[Id::INSTR_ADD].init(&instrAdd, "instrAdd");
	LIST[Id::INSTR_SUB].init(&instrSub, "instrSub");
	LIST[Id::INSTR_MUL].init(&instrMul, "instrMul");
	LIST[Id::INSTR_DIV].init(&instrDiv, "instrDiv");
	LIST[Id::INSTR_MOD].init(&instrMod, "instrMod");
	LIST[Id::INSTR_JMZ].init(&instrJmz, "instrJmz");
	LIST[Id::INSTR_HLT].init(&instrHlt, "instrHlt");

	LIST[Id::MACHINE_PROL].init(&machineProl, "machineProl");
	LIST[Id::MACHINE_EPIL].init(&machineEpil, "machineEpil");

	LIST[Id::INSTR_PROL].init(&instrProl, "instrProl");
	LIST[Id::LAST_INSTR_EPIL].init(&lastInstrEpil, "lastInstrEpil");
}

void PrototypeFuncs_x64::initInstructionCodeSize() {
	size_t max = 0;
	for (size_t i = 0; i < Operation::COUNT; i++) {
		size_t size = LIST[i].size;

		if (size > max) {
			max = size;
		}
	}

	specificInstrMaxCodeSize = max;

	instructionCodeSize = LIST[INSTR_PROL].size + specificInstrMaxCodeSize;
}

void PrototypeFuncs_x64::Info::init(PrototypeFunc* f, char const* name) {
	funcPtr = f;

	begin = getCodeBegin(f);
	uint8_t* end = getCodeEnd(f);
	size = end - begin;

	this->name = name;

	initPlaceholderOffsets();
}

void PrototypeFuncs_x64::Info::initPlaceholderOffsets() {
	uint8_t* p = begin;
	uint8_t* end = begin + size;
	size_t i = 0;
	for (; i < PLACEHOLDER_OFFSET_COUNT_MAX; i++) {
		p = findPlaceholderAddr(p, end);

		if (p == NULL) {
			break;
		}

		placeholderOffset[i] = p - begin;

		p++;
	}
	for (; i < PLACEHOLDER_OFFSET_COUNT_MAX; i++) {
		placeholderOffset[i] = -1;
	}
}

void PrototypeFuncs_x64::printDetails() {
	printf("leaRipRaxCodeSize: 0x%lx\n", leaRipRaxCodeSize);
	printf("standardFuncPrologueCodeSize: 0x%lx\n",
			standardFuncPrologueCodeSize);
	printf("specificInstrMaxCodeSize: 0x%lx\n", specificInstrMaxCodeSize);
	printf("instructionCodeSize: 0x%lx\n", instructionCodeSize);
	printf("--------------------------------\n");
	printf("prototype function list\n");
	printf("%s\t%s\t%s", "name     ", "size", "placeholder offsets\n");

	for (size_t i = 0; i < Id::COUNT; i++) {
		printf("%s\t0x%lx\t[ ", LIST[i].name, LIST[i].size);

		for (size_t j = 0; j < Info::PLACEHOLDER_OFFSET_COUNT_MAX; j++) {
			int16_t v = LIST[i].placeholderOffset[j];
			if (v != -1) {
				printf("%02x ", v);
			} else {
				printf("%d ", v);
			}
		}

		printf("]\n");
	}
}

void PrototypeFuncs_x64::initLeaRipRaxCodeSize() {
	register uint64_t begin asm ("rdx");
	register uint64_t end asm ("rax");

// store %rip at two times, once into %rdx and once into %rax
	/*...*/asm("lea (%rip),%rdx");
	/*rdx*/asm("lea (%rip),%rax");
	/*rax*/

	leaRipRaxCodeSize = end - begin;
}

void __attribute__((optimize("O0"))) PrototypeFuncs_x64::initStandardFuncPrologueCodeSize() {
// this source file describes a compiler, which I will call the inner compiler. this
// inner compiler needs to be compiled, too. that's the job of the outer compiler,
// e.g. gcc.
// this function, just like initLeaRipRaxCodeSize(), uses assembler instructions
// together with the instruction pointer to find out how big the function prologue
// ( https://en.wikipedia.org/wiki/Function_prologue ) is of functions which are a
// product of compiling with the outer compiler.
// knowing the length of the function prologue is necessary for extracting only the
// code body of the function without the prologue, which in turn is useful for the
// inner compiler, because the inner compiler makes use of the code generated by the
// outer compiler when the latter one compiled prototype functions.

	register uint64_t result asm ("rax");

// execute all following, but make compiler think that the if was needed.
// this is necessary because similar code exists in all prototype functions and we
// need to know this prologue-part's size. for some prototype functions, it is
// important to skip their code body, because stuff like jmp can't be executed
// there.
	volatile bool false_ = false;
	if (false_) {
		goto codeBodyEnd;
	}

	asm("lea (%rip),%rax");

	codeBodyEnd:

	// result -= (uint64_t) &PrototypeFuncs_x64::initStandardFuncPrologueCodeSize;
	result -= (uint64_t) &initStandardFuncPrologueCodeSize;
	result -= leaRipRaxCodeSize;

	standardFuncPrologueCodeSize = result;
}

uint8_t* PrototypeFuncs_x64::findPlaceholderAddr(uint8_t* begin, uint8_t* end) {
	uint8_t* p = begin;

	for (; p < end; ++p) {
		if ((*((uint64_t*) p) == PLACEHOLDER_Q)
				|| (*((uint32_t*) p) == PLACEHOLDER_D)) {
			return p;
		}
	}

	return NULL;
}

uint8_t* PrototypeFuncs_x64::getCodeBegin(PrototypeFunc* protoFunc) {
	return ((uint8_t*) protoFunc) + standardFuncPrologueCodeSize;
}

uint8_t* PrototypeFuncs_x64::getCodeEnd(PrototypeFunc* protoFunc) {

	uint8_t* misplacedEnd = protoFunc();

	return misplacedEnd - leaRipRaxCodeSize;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::machineProl() {
// first integer args are in:
// linux, solaris, freebsd, macosx: rdi, rsi, rdx, rcx, r8, r9
// windows........................: rcx, rdx, r8, r9

// %rbx, %rbp, and %r12-%r15 are callee-save, meaning the called subroutine
// either may not touch them or it has to restores them before returning.
// %rsp is hardwired to stack operations and for this purpose it initially
// points to the return address, so it must also be handled with care by
// a called subroutine.

// Typical Uses of Registers
//    EAX 	32-bit 	Accumulator for operands and results
//    EBX 	32-bit 	Base pointer to data in the data segment
//    ECX 	32-bit 	Counter for loop operations
//    EDX 	32-bit 	Data pointer and I/O pointer
//    EBP 	32-bit 	Frame Pointer - useful for stack frames
//    ESP 	32-bit 	Stack Pointer - hardcoded into PUSH and POP operations
//    ESI 	32-bit 	Source Index - required for some array operations
//    EDI 	32-bit 	Destination Index - required for some array operations
//    EIP 	32-bit 	Instruction Pointer
//    EFLAGS 	32-bit 	Result Flags - hardcoded into conditional operations

// use of registers by this compiler:
// rax: general purpose, usually direct values
// rbx
// rcx: step counter
// rdx: general purpose
// rbp
// rsp
// rsi: maximum step count, state.phase at end of run
// rdi: address of machine
// r8 : pointer to machine memory (address of first element's first byte)
// r9 : size of machine memory in quadwords
// r10: general purpose
// r11: general purpose, usually pointers
// r12
// r13
// r14
// r15

	register uint8_t* result asm ("rax");

// do not execute, but do not let compiler know this fact
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

// a
// b
// c
// d
// e
// f
// g 6
// h 4
// i 1
// j 7
// k 4
// l 1
// m 111
// n 2
// o 0
// p 9
// q 9
// r 8
// s 5
// t 7
// u 2
// v 7
// w 777
// x 4
// y 9
// z 2
// ' ' 0
// $0x111ac412e0980109 machine prolog
// $0x111ac412e0e91109 machine epilog
// $0x1257809801090000 instr prolog
// $0x1257804170000000 instr hlt
// $0x1257805777900000 instr swp
// $0x12578012c0000000 instr inc
// $0x125780dec0000000 instr dec
// $0x125780add0000000 instr add
// $0x12578052b0000000 instr sub
// $0x1257801112100000 instr mul
// $0x125780d170000000 instr div
// $0x1257801110d00000 instr mod
// $0x1257801111200000 instr jmz
	asm("mov $0x111ac412e0980109,%r11 # begin marker");

#if defined(__linux__) || defined(__APPLE__) || defined(__unix__)
// asm("mov %rdi,%rdi # machine address");
// asm("mov %rsi,%rsi # maximum stepCount");
#elif defined(_WIN64)
	asm("mov %rcx,%rdi # machine address");
	asm("mov %rdx,%rsi # maximum stepCount");
#else
#error "Unknown environment. The environment for which you are compiling isn't known. Thus, the assembly code can't be generated."
#endif

	asm("mov 0x00(%rdi),%rcx # stepCount");
	asm("mov 0x18(%rdi),%r8  # machine memory address");
	asm("mov 0x20(%rdi),%r9  # machine memory size in quadwords");

	asm("cmp %rcx,%rsi # compare step counter to step counter limit");
	asm("jne stepCounterInRunningRange");
	asm("retq");
	asm("stepCounterInRunningRange:");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rax # instrCount");
	asm("cmp %rax,0x08(%rdi) # ensure state.instrPtr less than instrCount");
	asm("jl instrPtrInRunningRange");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rax # FINISHED");
	asm("mov %rax,0x10(%rdi)");
	asm("retq");
	asm("instrPtrInRunningRange:");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rax # RUNNABLE");
	asm("cmp %rax,0x10(%rdi) # ensure phase is RUNNABLE");
	asm("je stateIsRunnable");
	asm("retq");
	asm("stateIsRunnable:");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rax # instructionCodeSize");
	asm("imul 0x08(%rdi),%rax                   # state.instrPtr * instructionCodeSize is the offset in the machine's native code where to resume execution");
	asm("lea 0x5(%rip),%rdx                     # 0x5: size of following instructions add and jmp");
	asm("add %rdx,%rax");
	asm("jmp *%rax");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd:

// store end
	asm("lea (%rip),%rax");

	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::machineEpil() {
	register uint8_t* result asm ("rax");

// do not execute, but do not let compiler know this fact
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x111ac412e0e91109,%r11 # begin marker");

	asm("mov %rcx,0x00(%rdi) # stepCount");
	asm("mov %rdx,0x08(%rdi) # state.instrPtr");
	asm("mov %rsi,0x10(%rdi) # state.phase");

	asm("retq");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd:

// store end
	asm("lea (%rip),%rax");

	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrProl() {
	register uint8_t* result asm ("rax");

// do not execute, but do not let compiler know this fact
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1257809801090000,%r11 # begin marker");

	asm("cmp %rcx,%rsi # compare step counter to step counter limit");
	asm("jne continueExecution");

	asm("lea 0x2(%rip),%rax # will lea absolute address of following placeholder");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # address of machine epilogue, relative to this placeholder");
	asm("add %rdx,%rax # absolute address of machine epilogue");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # instrIdx of current instruction");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rsi # RUNNABLE");

	asm("jmp *%rax # jump to machine epilogue");

	asm("continueExecution:");
	asm("inc %rcx # increase the step counter");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd:

// store end
	asm("lea (%rip),%rax");

	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::lastInstrEpil() {
	register uint8_t* result asm ("rax");

// do not execute, but do not let compiler know this fact
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1a570125780e9110,%r11 # begin marker");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # instrCount = instrIdx of instruction behind last valid one");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rsi # FINISHED");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd:

// store end
	asm("lea (%rip),%rax");

	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrHlt() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1257804170000000,%r11 # begin marker");

	asm("lea 0x2(%rip),%rax # will lea absolute address of following placeholder");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx");
	asm("add %rdx,%rax # keep addr of machine epilogue in here");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # instrIdx of current instruction");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rsi # FINISHED");

	asm("jmp *%rax");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrSwp() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1257805777900000,%r11 # begin marker");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx");
	asm("lea (%r8,%rdx,8),%rdx");
	asm("mov (%rdx),%rax");
	asm("xchg %rax,(%r8)");
	asm("mov %rax,(%rdx)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrInc() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x12578012c0000000,%r11 # begin marker");

	asm("incq (%r8)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrDec() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x125780dec0000000,%r11 # begin marker");

	asm("decq (%r8)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrAdd() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x125780add0000000,%r11 # begin marker");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx");
	asm("mov (%r8,%rdx,8),%rax");
	asm("add %rax,(%r8)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrSub() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x12578052b0000000,%r11 # begin marker");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx");
	asm("mov (%r8,%rdx,8),%rax");
	asm("sub %rax,(%r8)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrMul() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1257801112100000,%r11 # begin marker");

	asm("mov (%r8),%rax         # load regst[0]");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx");
	asm("imul (%r8,%rdx,8),%rax # multiply %rax with regst[param0]");
	asm("mov %rax,(%r8)         # store result to regst[0]");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrDiv() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x125780d170000000,%r11 # begin marker");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%r11 # regstIdx of divisor");
	asm("mov (%r8,%r11,8),%r10");

	asm("test %r10,%r10");
	asm("jne nonzeroDiv");

	asm("lea 0x2(%rip),%rax # will lea absolute address of following placeholder");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # address of machine epilogue, relative to this placeholder");
	asm("add %rdx,%rax # absolute address of machine epilogue");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # instrIdx of current instruction");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rsi # ABORTED_DIVZERO");

	asm("jmp *%rax");
	// TODO: jumps to location way before
	// machine epilogue. this is wrong. find out why.

	asm("nonzeroDiv:");
	asm("mov (%r8),%rax");
	asm("cqto");
	asm("idivq %r10");
	asm("mov %rax,(%r8)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker" );

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrMod() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1257801110d00000,%r11 # begin marker");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%r11 # regstIdx of divisor");
	asm("mov (%r8,%r11,8),%r10");

	asm("test %r10,%r10");
	asm("jne nonzeroMod");

	asm("lea 0x2(%rip),%rax # will lea absolute address of following placeholder");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # address of machine epilogue, relative to this placeholder");
	asm("add %rdx,%rax # absolute address of machine epilogue");

	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx # instrIdx of current instruction");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rsi # ABORTED_DIVZERO");

	asm("jmp *%rax");

	asm("nonzeroMod:");
	asm("mov (%r8),%rax");
	asm("cqto");
	asm("idivq %r10");
	asm("mov %rdx,(%r8)");

	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker" );

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

uint8_t* __attribute__((optimize("O0"))) PrototypeFuncs_x64::instrJmz() {
	register uint8_t* result asm ("rax");
	volatile bool true_ = true;
	if (true_) {
		goto codeBodyEnd;
	}

	asm("mov $0x1257807111200000,%r11 # begin marker");
	asm("cmpq $0x0,(%r8)");
	asm("jne dontJump");

	asm("lea 0x2(%rip),%rax");
	asm("mov $" XSTR(PLACEHOLDER_Q_BARE) ",%rdx");
	asm("add %rdx,%rax");
	asm("jmp *%rax");

	asm("dontJump:");
	asm("mov $0xeeeeeeeeeeeeeeee,%r11 # end marker");

	codeBodyEnd: //
	asm("lea (%rip),%rax");
	return result;
}

size_t PrototypeFuncs_x64::leaRipRaxCodeSize = (size_t) -1;
size_t PrototypeFuncs_x64::standardFuncPrologueCodeSize = (size_t) -1;
size_t PrototypeFuncs_x64::specificInstrMaxCodeSize = (size_t) -1;
size_t PrototypeFuncs_x64::instructionCodeSize = (size_t) -1;

PrototypeFuncs_x64::Info PrototypeFuncs_x64::LIST[PrototypeFuncs_x64::Id::COUNT];

} /* namespace agos */
