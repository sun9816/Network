#include <iostream>
#include <vector>
#include <string>
#include "String.h"
using namespace std;
using namespace Network;
int main(void) {
	String str("helloworled");
	cout << str.substr("l", "d", 4).stdstr();
	return 0;
}