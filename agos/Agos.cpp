#include "Agos.h"

#include <sys/types.h>
#include <iostream>
#include <string>
#include <chrono>

#include "compiler/x64/Compiler_x64.h"
#include "goedelnumbering/multiplier/MachineGoedeler.h"
#include "Int64ToInt64Function.h"
#include "machine/Machine.h"
#include "machine/MachineFactory.h"
#include "machine/Program.h"
#include "stringformat.h"

namespace agos {

using std::cout;
using std::endl;

bool testNativeRun(Machine &m) {
	size_t maxSteps = 1000;

	m.compile();

	for (int64_t i = 0; i < 100; i++) {

		// cout << "input: " << i << endl;

		m.reset();
		m.setRegst(0, i);
		m.run(maxSteps);
		Machine::State interp = m.getState();

		m.reset();
		m.setRegst(0, i);
		m.runNatively(maxSteps);
		Machine::State native = m.getState();

		bool failure = (native != interp);

		if (failure) {
			cout << "interp: " << interp.toString() << endl;
			cout << "native: " << native.toString() << endl;
			cout << "input: " << i << endl;
			return failure;
		}

	}

	return false;
}

void testNativeRun() {
	Compiler::getLocalInstance();

	Machine m;

	MachineGoedeler &machGoed = MachineGoedeler::getInstance();
	machGoed.incrToNoKnownUselessInstr(m);

	bool nextLayout = true;

	size_t i = 0;

	for (; i < 1; i++) {
		nextLayout = machGoed.incr(m);
	}

	while (true) {
		cout << i << " m: " << m.toString() << endl;

		bool failure = testNativeRun(m);
		if (failure) {
			cout << i << " failed: " << m.toStringMultiline() << endl;
			cout << endl;
			break;
		}

		nextLayout = machGoed.incr(m);
		if (nextLayout) {
			cout << "---- next layout: " << m.toStringBasic() << endl;
		}

		i++;
		if (i >= 500000) {
			break;
		}
	}
}

void testMachine() {
	Machine m = MachineFactory::createIsNotMultipleOf3();
	Int64ToInt64Function const &f = Int64ToInt64Function::getIsNotMultipleOf3();

	cout << "target : " << f.getName() << endl;
	cout << "machine: " << m.toStringMultiline() << endl;

	cout << endl;
	if (!test(m, f)) {
		cout << "fail" << endl;
	} else {
		cout << "correct" << endl;
	}

	cout << endl;
	for (int64_t input = 0; input < 10; input++) {
		m.reset();
		m.setRegst(0, input);

		m.run(100);

		cout << input << ": ";
		if (m.getPhase() == Machine::State::Phase::FINISHED) {
			cout << m.getRegst(0) << endl;
		} else {
			cout << m.getPhase().toString() << endl;
		}
	}
}

decltype(std::chrono::system_clock::now()) currentInstant() {
	return std::chrono::system_clock::now();
}

int64_t duration(
		std::chrono::time_point<std::chrono::_V2::system_clock,
				std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > instantA,
		std::chrono::time_point<std::chrono::_V2::system_clock,
				std::chrono::duration<long int, std::ratio<1l, 1000000000l> > > instantB) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(instantB - instantA).count();
}

void findMachine() {
	Machine m;
	Int64ToInt64Function const &f = Int64ToInt64Function::getIsNotMultipleOf3();

	cout << "target: " << f.getName() << endl;

	MachineGoedeler &machGoed = MachineGoedeler::getInstance();
	machGoed.incrToNoKnownUselessInstr(m);

	auto startInstant = currentInstant();
	auto lastOutput = startInstant;

	bool nextLayout = true;

	size_t i = 0;
	while (true) {
		if (nextLayout) {
			cout << "---- next layout: " << m.toStringBasic() << endl;
		}

		if (i % 10000 == 0) {
			auto currInstant = currentInstant();

			auto durationSinceOutput = duration(lastOutput, currInstant);
			if (durationSinceOutput >= 1000) {
				lastOutput = currInstant;
				double secSinceStart = duration(startInstant, currInstant) / 1000;
				size_t machPerSec = i / secSinceStart;
				cout << secSinceStart << "sec uptime; " << //
						machPerSec << " m/sec; " << //
						i << " tested" << //
						endl;
			}
		}

		bool success = test(m, f);
		if (success) {
			cout << endl;
			cout << "success: " << f.getName() << endl;
			cout << m.toStringMultiline() << endl;
			break;
		}

		nextLayout = machGoed.incr(m);

		i++;
		if (i >= 5000000000) {
			break;
		}
	}
}

bool test(Machine &m, Int64ToInt64Function const &f) {
	for (int64_t input = 0; input < 100; input++) {
		m.reset();
		m.setRegst(0, input);

		m.run(100);

		if (m.getPhase() != Machine::State::Phase::FINISHED) {
			return false;
		}

		int64_t desiredOutput = f.apply(input);
		int64_t actualOutput = m.getRegst(0);

		if (actualOutput != desiredOutput) {
			return false;
		}
	}

	return true;
}

void findMachineNative() {
	// Compiler* compiler = Compiler::getLocalInstance();

	Machine m;
	Int64ToInt64Function const &f = Int64ToInt64Function::getIsNotMultipleOf3();

	cout << "target: " << f.getName() << endl;

	MachineGoedeler &machGoed = MachineGoedeler::getInstance();
	machGoed.incrToNoKnownUselessInstr(m);

	auto startInstant = currentInstant();
	auto lastOutput = startInstant;

	bool nextLayout = true;

	size_t i = 0;
	while (true) {
		if (nextLayout) {
			cout << "---- next layout: " << m.toStringBasic() << endl;
			m.compile();
		}

		if (i % 10000 == 0) {
			auto currInstant = currentInstant();

			auto durationSinceOutput = duration(lastOutput, currInstant);
			if (durationSinceOutput >= 1000) {
				lastOutput = currInstant;
				double secSinceStart = duration(startInstant, currInstant) / 1000;
				size_t machPerSec = i / secSinceStart;
				cout << secSinceStart << "sec uptime; " << //
						machPerSec << " m/sec; " << //
						i << " tested" << //
						endl;
			}
		}

		bool success = testNative(m, f);
		if (success) {
			cout << endl;
			cout << "success: " << f.getName() << endl;
			cout << m.toStringMultiline() << endl;
			break;
		}

		size_t modifiedInstrCount;
		nextLayout = machGoed.incr(m, &modifiedInstrCount);
		if (!nextLayout) {
			for (size_t instrIdx = 0; instrIdx < modifiedInstrCount; ++instrIdx) {
				m.compileInstr(instrIdx);
			}
		}

		i++;
		if (i >= 5000000000) {
			break;
		}
	}
}

bool testNative(Machine &m, Int64ToInt64Function const &f) {
	for (int64_t input = 0; input < 100; input++) {
		m.reset();
		m.setRegst(0, input);

		m.runNatively(100);

		if (m.getPhase() != Machine::State::Phase::FINISHED) {
			return false;
		}

		int64_t desiredOutput = f.apply(input);
		int64_t actualOutput = m.getRegst(0);

		if (actualOutput != desiredOutput) {
			return false;
		}
	}

	return true;
}

void mainAgos() {
	// testNativeRun();
	//findMachine();
	//findMachineNative();

	Machine m = MachineFactory::createTimes5Plus1();
	m.compile();

	m.setRegst(0, 6);
	m.runNatively(100);
	cout << m.getRegst(0) << endl;
}

} /* namespace agos */
