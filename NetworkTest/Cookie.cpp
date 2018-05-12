#include "Cookie.h"
#include <set>
#include <ctime>
#include <algorithm>

using namespace std;
using namespace Network;

#define COOKIE_SECURE			"secure"
#define COOKIE_HTTPONLY			"httponly"
#define COOKIE_DOMAIL			"domain"
#define COOKIE_PATH				"path"
#define COOKIE_MAXAGE			"max-age"
#define COOKIE_EXPIRES			"expires"


Cookie::Cookie() {
	_initMap();
}

Cookie::Cookie(const String & _Str, const Url & _Url) {
	_initMap();
	setRawString(_Str, _Url);
}
Cookie::Cookie(const Cookie & _Cookie) :
	m_strName(_Cookie.m_strName) ,
	m_strValue(_Cookie.m_strValue) ,
	m_strDomain(_Cookie.m_strDomain),
	m_strPath(_Cookie.m_strPath),
	m_timeExpires(_Cookie.m_timeExpires),
	m_bHttpOnly(_Cookie.m_bHttpOnly),
	m_bSecure(_Cookie.m_bSecure) {
	_initMap();
}

Cookie::~Cookie() {
}

bool Cookie::setRawString(const String & _Str, const Url & _Url) {
	_setDefault();

	StringList list = _Str.split(";");
	if (list.empty()) {
		return false;
	}
	StringList kvlist = list[0].split("=", 2);
	if (kvlist.size() != 2) {
		return false;
	}
	m_strName = kvlist[0].trim();
	m_strValue = kvlist[1].trim();
	if (m_strName.empty()
		|| m_strValue.empty()) {
		return false;
	}

	for (size_t i = 1; i < list.size(); ++i) {
		kvlist = list[i].split("=", 2);
		if (m_mapKv[kvlist[0].trim().lower()]) {
			m_mapKv[kvlist[0].trim().lower()](kvlist[1].trim());
		}
	}

	if (m_strDomain.empty()) {
		m_strDomain = _Url.host().left(4) != "www." ? _Url.host() : _Url.host().mid(4);
	}

	if (m_strPath.empty()) {
		m_strPath = _Url.path();
	}

	return true;
}

String Cookie::name() const {
	return m_strName;
}

String Cookie::value() const {
	return m_strValue;
}

String Cookie::domain() const {
	return m_strDomain;
}

String Cookie::path() const {
	return m_strPath;
}

time_t Cookie::expires() const {
	return m_timeExpires;
}

bool Cookie::isHttpOnly() const {
	return m_bHttpOnly;
}

bool Cookie::isSecure() const {
	return m_bSecure;
}

bool Network::Cookie::operator==(const Cookie & _Cookie) const {
	return m_strName == _Cookie.m_strName && m_strValue == _Cookie.m_strValue;
}

bool Network::Cookie::operator!=(const Cookie & _Cookie) const {
	return !operator==(_Cookie);
}

Cookie & Network::Cookie::operator=(const Cookie & _Cookie) {
	m_strName = _Cookie.m_strName;
	m_strValue = _Cookie.m_strValue;
	m_strDomain = _Cookie.m_strDomain;
	m_strPath = _Cookie.m_strPath;
	m_timeExpires = _Cookie.m_timeExpires;
	m_bHttpOnly = _Cookie.m_bHttpOnly;
	m_bSecure = _Cookie.m_bSecure;
	return *this;
}

void Cookie::_setDefault() {
	m_strName.clear();
	m_strValue.clear();
	m_strDomain.clear();
	m_strPath.clear();
	m_timeExpires = 0;
	m_bHttpOnly = false;
	m_bSecure = false;
}

void Cookie::_initMap() {
	m_mapKv[COOKIE_SECURE] = [&](const String & value) {
		m_bSecure = true;
	};
	m_mapKv[COOKIE_HTTPONLY] = [&](const String & value) {
		m_bHttpOnly = true;
	};
	m_mapKv[COOKIE_DOMAIL] = [&](const String & value) {
		if (!value.empty()) {
			if (value[0] != '.') {
				m_strDomain.push_back('.');
			}
			m_strDomain.append(value);
		}
	};
	m_mapKv[COOKIE_PATH] = [&](const String & value) {
		m_strPath = value;
	};
	m_mapKv[COOKIE_MAXAGE] = [&](const String & value) {
		m_timeExpires = time(nullptr) + value.toInt();
	};
	m_mapKv[COOKIE_EXPIRES] = [&](const String & value) {
		_setGMTtime(value);
	};
}

void Cookie::_setGMTtime(const String & _Str) {
	static const vector<String> setMonth = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	char szMonth[5] = { 0 };
	tm t = { 0 };

	if (sscanf_s(_Str.stdstr().c_str(), "%*[^, ]%*[, ]%2d%*[-/ ]%[^-/ ]%*[-/ ]%4d %2d:%2d:%2d %*s",
		&t.tm_mday, szMonth, _countof(szMonth), &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec) != 7) {
		return;
	}

	if (t.tm_year < 70) {
		t.tm_year += 100;
	}
	else if (t.tm_year > 100) {
		t.tm_year -= 1900;
	}

	auto it = setMonth.end();
	if ((it = find_if(setMonth.begin(), setMonth.end(), [&](const String & v) {return v == szMonth; })) == setMonth.end()) {
		return;
	}
	t.tm_mon = it - setMonth.begin();

	m_timeExpires = mktime(&t);
}

String CookieList::join(const String & _Sep) const {
	String ret;
	bool isfirst = true;
	for (auto & it : *this) {
		if (isfirst) {
			isfirst = false;
		}
		else {
			ret += _Sep;
		}
		ret += it.name() + "= " + it.value();
	}
	return ret;
}
