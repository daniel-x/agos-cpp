#ifndef AGOS_H_
#define AGOS_H_

#include "machine/Machine.h"
#include "Int64ToInt64Function.h"

namespace agos {

void mainAgos();
bool test(Machine& m, Int64ToInt64Function const& f);
bool testNative(Machine& m, Int64ToInt64Function const& f);

} /* namespace agos */

#endif /* AGOS_H_ */
