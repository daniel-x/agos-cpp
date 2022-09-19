#include "stringformat.h"

#include <cstdarg>
#include <sstream>
#include <cmath>
#include <limits>

using std::va_list;

namespace agos {

using std::string;
using std::ostringstream;

string strfmt(const string fmt, ...) {
	int size = ((int) fmt.size()) * 2 + 50; // Use a rubric appropriate for your code
	string str;
	va_list ap;
	while (1) {     // Maximum two passes on a POSIX system...
		str.resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char *) str.data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {  // Everything worked
			str.resize(n);
			return str;
		}
		if (n > -1)  // Needed size returned
			size = n + 1;   // For null char
		else
			size *= 2;      // Guess at a larger size (OS specific)
	}
	return str;
}

//string toString(BigInteger v) {
//	ostringstream oss;
//	oss << v;
//	return oss.str();
//}

//std::string toStringWithSuffix(size_t v) {
//	BigInteger vBigInteger = v;
//
//	return toStringWithSuffix(vBigInteger);
//}
//
//std::string toStringWithSuffix(BigInteger v) {
//	ostringstream oss;
//
//	char suffixAr[] = "1KMGTPEZY";
//
//	if (v < 0) {
//		oss << '-';
//		v = -v;
//	}
//
//	size_t j;
//	double vd = -1;
//	for (j = 0; j < std::numeric_limits<std::size_t>::max(); j++) {
//		if (v < 1000000) {
//			vd = v.toLong();
//			if (v >= 1000) {
//				j++;
//				vd /= 1000;
//			}
//
//			break;
//		}
//
//		v /= 1000;
//	}
//
//	if (vd >= 100) {
//		vd = std::round(vd);
//	} else if (vd >= 10) {
//		vd = std::round(vd * 10) / 10.0;
//	} else if (vd >= 1) {
//		vd = std::round(vd * 100) / 100.0;
//	}
//
//	int vdi = (int) vd;
//	if (vd == vdi) {
//		oss << vdi;
//	} else {
//		oss.precision(4);
//		oss << vd;
//	}
//
//	if (j > 0) {
//		if (j <= 8) {
//			oss << suffixAr[j];
//		} else {
//			oss << suffixAr[8];
//			j -= 8;
//
//			oss << "*1000^" << j;
//		}
//	}
//
//	return oss.str();
//}

} /* namespace agos */
