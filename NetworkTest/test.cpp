#include <iostream>
#include <vector>
#include <string>
#include "String.h"
#include "Url.h"
using namespace std;
using namespace Network;


int main(void) {
	cout << Url::encodeURIComponent("https://www.baidu.com/ a b c").stdstr();
	return 0;
}