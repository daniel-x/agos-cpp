#include <cstdint>
#include <string>

#ifndef INT64TOINT64FUNCTION_H_
#define INT64TOINT64FUNCTION_H_

namespace agos {

typedef int64_t (Int64ToInt64BareFunction)(int64_t);

class Int64ToInt64Function {
protected:
	const Int64ToInt64BareFunction* bare;
	const std::string name;

public:
	Int64ToInt64Function(Int64ToInt64BareFunction* bare, std::string name) :
			bare(bare), name(name) {
	}

	int64_t apply(int64_t x) const {
		return bare(x);
	}

	std::string getName() const {
		return name;
	}

	static Int64ToInt64Function getIsNotMultipleOf3();
	static Int64ToInt64Function getIsOdd();
};

} /* namespace agos */

#endif /* INT64TOINT64FUNCTION_H_ */
