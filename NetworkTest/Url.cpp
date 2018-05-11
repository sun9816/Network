#include "Url.h"
#include <regex>
#include <cctype>

using namespace std;
using namespace Network;


#define DEC_TO_HEX(x)	(((x)>=10)?'A'+(x)-10:'0'+(x))
#define HEX_TO_DEC(x)	(::isdigit(x)?(x)-'0':(::toupper(x)-'A'+10))

enum URL_NAME {
	UN_URL		, 
	UN_SCHEME	,
	UN_SLASH	,
	UN_HOST		,
	UN_PORT		,
	UN_PATH		,
	UN_QUERY	,
	UN_HASH
};

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
	_setDefault();
}

Url::Url(const String & _Str) {
	setUrl(_Str);
}

Url::Url(const Url & _Url) {
	m_strUrl = _Url.m_strUrl;
	m_strScheme = _Url.m_strScheme;
	m_strHost = _Url.m_strHost;
	m_strPort = _Url.m_strPort;
	m_strPath = _Url.m_strPath;
	m_strQuery = _Url.m_strQuery;
	m_strHash = _Url.m_strHash;
}

Url::~Url() {
}

bool Url::setUrl(const String & _Str) {
	regex re(R"(^(?:([A-Za-z]+):)?(\/{0,3})([0-9.\-A-Za-z]+)(?::(\d+))?(?:\/([^?#]*))?(?:\?([^#]*))?(?:#(.*))?$)");
	string str = _Str.stdstr();
	smatch sma;

	if (!regex_match(str, sma, re)
		|| sma.size() != 8) {
		return false;
	}

	m_strUrl = sma.str(UN_URL);
	m_strScheme = sma.str(UN_SCHEME);
	m_strHost = sma.str(UN_HOST);
	m_strPort = sma.str(UN_PORT).empty() ? (m_strScheme.lower() == "http" ? "80" : "443") : sma.str(UN_PORT);
	m_strPath = "/" + sma.str(UN_PATH);
	m_strQuery = sma.str(UN_QUERY);
	m_strHash = sma.str(UN_HASH);
	return true;
}

String Url::url() const {
	return m_strUrl;
}

String Url::scheme() const {
	return m_strScheme;
}

String Url::host() const {
	return m_strHost;
}

String Url::port() const {
	return m_strPort;
}

String Url::path() const {
	return m_strPath;
}

String Url::query() const {
	return m_strQuery;
}

String Url::hash() const {
	return m_strHash;
}

void Url::_setDefault() {
	m_strUrl.clear();
	m_strScheme.clear();
	m_strHost.clear();
	m_strPort.clear();
	m_strPath.clear();
	m_strQuery.clear();
	m_strHash.clear();
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
