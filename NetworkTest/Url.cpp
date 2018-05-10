#include "Url.h"
#include <regex>
#include <cctype>

using namespace std;
using namespace Network;


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

Url::Url() {
}

Url::Url(const String & _Str) {
	setUrl(_Str);
}

Url::Url(const Url & _Url) :
	m_mapUrl(_Url.m_mapUrl) {
}

Url::~Url() {
}

bool Url::setUrl(const String & _Str) {
	static const char *names[] = { URL_URL, URL_SCHEME, URL_SLASH, URL_HOST, URL_PORT, URL_PATH, URL_QUERY, URL_HASH };

	regex re(R"(^(?:([A-Za-z]+):)?(\/{0,3})([0-9.\-A-Za-z]+)(?::(\d+))?(?:\/([^?#]*))?(?:\?([^#]*))?(?:#(.*))?$)");
	string str = _Str.stdstr();
	smatch sma;

	m_mapUrl.clear();
	if (!regex_match(str, sma, re)
		|| sma.size() != 8) {
		return false;
	}

	for (size_t i = 0; i < sma.size(); ++i) {
		m_mapUrl[names[i]] = sma.str(i);
	}
	return true;
}

void Url::setValue(const String & _Name, const String & _Str) {
	m_mapUrl[_Name] = _Str;
}

String Network::Url::value(const String & _Name) {
	String str = m_mapUrl[_Name];
	if (!str.empty()) {
		return str;
	}

	if (_Name == URL_PORT) {
		if (m_mapUrl[URL_SCHEME].lower() == "http") {
			return String("80");
		}
		if (m_mapUrl[URL_SCHEME].lower() == "https") {
			return String("443");
		}
		return String("");
	}

	if (_Name == URL_PATH) {
		return String("/");
	}

	return String("");
}

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
