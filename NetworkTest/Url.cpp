#include "Url.h"
#include <cctype>
using namespace std;
using namespace Network;

Url::Url() {
}

Url::~Url() {
}

#define DEC_TO_HEX(x)	(((x)>=10)?'A'+(x)-10:'0'+(x))
#define HEX_TO_DEC(x)	(::isdigit(x)?(x)-'0':(::toupper(x)-'A'+10))

static const char _encodeURITables[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,0,0,1,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

static const char _encodeURIComponentTables[] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,1,1,0,0,
	1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };

String Url::encodeURI(const String & _Str) {
	return encode(_Str, _encodeURITables);
}

String Url::decodeURI(const String & _Str) {
	return decode(_Str, _encodeURITables);
}

String Url::encodeURIComponent(const String & _Str) {
	return encode(_Str, _encodeURIComponentTables);
}

String Url::decodeURIComponent(const String & _Str) {
	return decode(_Str, _encodeURIComponentTables);
}

String Url::encode(const String & _Str, const char * _Tables) {
	String ret;

	for (auto & it : _Str) {
		if (_Tables[(unsigned char)it]) {
			ret.push_back('%');
			ret.push_back(DEC_TO_HEX((unsigned char)it >> 4));
			ret.push_back(DEC_TO_HEX((unsigned char)it & 0x0F));
		}
		else {
			ret.push_back(it);
		}
	}

	return ret;
}

String Url::decode(const String & _Str, const char *_Tables) {
	String ret;

	for (size_t i = 0; i < _Str.size(); ++i) {
		char ch = _Str[i];
		if (ch == '%') {
			if (i + 2 >= _Str.size()) {
				return String("");
			}

			char dec =  (HEX_TO_DEC(_Str[i + 1]) << 4) + (HEX_TO_DEC(_Str[i + 2]));
			if (_Tables[(unsigned char)dec]) {
				ch = dec;
				i += 2;
			}
		}
		ret.push_back(ch);
	}

	return ret;
}
