#include <iostream>
#include <string>
#include "agos/Agos.h"

using std::cout;
using std::endl;
using std::string;
using namespace agos;

int main() {
	try {
		mainAgos();
	} catch (string& e) {
		cout << "exception caught: " << e << endl;
	}

	return 0;
}
