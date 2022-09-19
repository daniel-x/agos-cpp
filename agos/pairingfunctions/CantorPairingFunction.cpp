#include "CantorPairingFunction.h"

#include "PairingFunction.h"

#include "../stringformat.h"

#include <cmath>
#include <iostream>
#include <string>

namespace agos {

using std::string;
using std::cout;
using std::endl;
using std::sqrt;
using std::ceil;

void CantorPairingFunction::incr(size_t* tuple, size_t elementCount) const {
	size_t* first = tuple;
	size_t* last = first + elementCount - 1;
	size_t* curr, *next;
	for (curr = first, next = first; curr < last; curr = next) {
		++next;

		if (*curr > 0) {
			--*curr;
			++*next;
			break;
		}
	}

	if (curr != last) {
		size_t prevSum = *first;
		size_t* decremented = curr;
		for (curr = first + 1; curr <= decremented; ++curr) {
			prevSum += *curr;
			*curr = 0;
		}
		*first = prevSum;
	} else {
		size_t tmp = *last + 1; // tmp necessary when t.length = 1
		*last = 0;
		*first = tmp;
	}
}

size_t CantorPairingFunction::encode(const size_t* tuple,
		size_t elementCount) const {
	size_t sum = PairingFunction::sum(tuple, elementCount);

	size_t b = sum * ((sum - 1L) / 2L * (elementCount - 1) + 1);

	size_t a = 0;

	for (size_t i = 0; i < elementCount - 1; i++) {
		size_t moveCount = elementCount - 1 - i;
		a += tuple[i] * moveCount;
	}

	return b + a;
}

size_t* CantorPairingFunction::decode(size_t n, size_t elementCount,
		size_t* resultTuple) const {
	int elementCountInt = elementCount;

	double m_p_2 = (elementCountInt - 3) / ((double) 2) / (elementCountInt - 1);

	double sq = m_p_2 * m_p_2 + ((double) 2) * n / (elementCountInt - 1);

	double sDouble = m_p_2 + std::sqrt(sq);

	int sum = (int) std::ceil(sDouble) - 1;

	int b = sum * ((sum - 1) / 2 * (elementCountInt - 1) + 1);

	int r = n - b;

	for (int i = 0; i < r; i++) {
		incr(resultTuple, elementCountInt);
	}

	return resultTuple;
}

void CantorPairingFunction::test() {
	CantorPairingFunction& tupleIter = CantorPairingFunction::getInstance();

	size_t t1[1] = { 0 };
	size_t t2[2] = { 0 };
	size_t t3[3] = { 0 };
	size_t t4[4] = { 0 };

	size_t t[10];

	string fmt = "%2s %6s %8s %11s %14s %4s %4s %4s %4s %11s";
	string header = strfmt(fmt, "n", "1", "2", "3", "4", "n1", "n2", "n3", "n4",
			"s");

	cout << header << endl;
	cout
			<< "---------------------------------------------------------------------------"
			<< endl;
	for (size_t n = 0; n < 19; n++) {
		string t1Str = toString(t1, 1);
		string t2Str = toString(t2, 2);
		string t3Str = toString(t3, 3);
		string t4Str = toString(t4, 4);

		size_t n1 = tupleIter.encode(t1, 1);
		size_t n2 = tupleIter.encode(t2, 2);
		size_t n3 = tupleIter.encode(t3, 3);
		size_t n4 = tupleIter.encode(t4, 4);

		tupleIter.decode(n, 2, t);
		string s = toString(t, 2);

		cout << n << "       " << t1Str << "      " << t2Str << "       "
				<< t3Str << "        " << t4Str << "    " << n1 << "    " << n2
				<< "    " << n3 << "    " << n4 << "     " << s << endl;

		tupleIter.incr(t1, 1);
		tupleIter.incr(t2, 2);
		tupleIter.incr(t3, 3);
		tupleIter.incr(t4, 4);
	}
}

}
/* namespace agos */
