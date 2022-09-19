#include "Compiler.h"
#include "x64/Compiler_x64.h"

namespace agos {

Compiler* Compiler::getLocalInstance() {
	static Compiler* INSTANCE = (Compiler*) Compiler_x64::getInstance();
	return INSTANCE;

}

void printHex(void const* begin, void const* end) {
	printf("%#018lx: ", (uint64_t) begin);

	for (unsigned char* p = (unsigned char*) begin; p < end; ++p) {
		if (p + 1 < end) {
			printf("%02x ", *p);
		} else {
			printf("%02x", *p);
		}
	}

	printf("\n");
}

} /* namespace agos */
