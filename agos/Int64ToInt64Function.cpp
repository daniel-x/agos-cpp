#include "Int64ToInt64Function.h"

namespace agos {

Int64ToInt64Function Int64ToInt64Function::getIsNotMultipleOf3() {
	static Int64ToInt64Function INSTANCE( //
			[](int64_t x) -> int64_t {
				return x % 3 == 0 ? 0 : 1;
			}, "IsNotMultipleOf3");

	return INSTANCE;
}

Int64ToInt64Function Int64ToInt64Function::getIsOdd() {
	static Int64ToInt64Function INSTANCE( //
			[](int64_t x) -> int64_t {
				return x % 2;
			}, "IsOdd");

	return INSTANCE;
}

} /* namespace agos */
