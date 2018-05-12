#include "CookieJar.h"
#include <algorithm>
using namespace std;
using namespace Network;

CookieJar::CookieJar() {
}

CookieJar::CookieJar(const CookieJar & _Cookiejar) :
	m_vecCookieJar(_Cookiejar.m_vecCookieJar) {
}

CookieJar::~CookieJar() {
}

CookieList CookieJar::cookieForUrl(const Url & _Url) const {
	//·µ»ØÆ¥Åädomail, pathµÄCookie
	CookieList list;

	for (auto & it : m_vecCookieJar) {
		if (_isDomain(it.domain(), _Url.host())
			&& _isPath(it.path(), _Url.path())) {
			list.push_back(it);
		}
	}

	return list;
}

bool CookieJar::updateCookie(const Cookie & _Cookie) {
	if (!deleteCookie(_Cookie)) {
		return false;
	}
	return insertCookie(_Cookie);
}

bool CookieJar::deleteCookie(const Cookie & _Cookie){
	auto it = find_if(m_vecCookieJar.begin(), m_vecCookieJar.end(), [&](const Cookie & _It) {
		return _Cookie == _It;
	});

	if (it != m_vecCookieJar.end()) {
		m_vecCookieJar.erase(it);
		return true;
	}
	return false;
}

bool CookieJar::insertCookie(const Cookie & _Cookie) {
	auto it = find_if(m_vecCookieJar.begin(), m_vecCookieJar.end(), [&](const Cookie & _It) {
		return _Cookie == _It;
	});

	if (it != m_vecCookieJar.end()) {
		return false;
	}

	m_vecCookieJar.push_back(_Cookie);
	return true;
}

CookieJar & CookieJar::operator=(const CookieJar & _Cookiejar) {
	m_vecCookieJar = _Cookiejar.m_vecCookieJar;
	return *this;
}

bool CookieJar::_isDomain(const String & _Left, const String & _Right) const {
	String domain = _Right.left(4) != "www." ? _Right : _Right.mid(4);

	if (domain.size() < _Left.size() - (_Left[0] == '.' ? 1 : 0)
		|| domain.size() < 2
		|| _Left.size() < 2) {
		return false;
	}

	if (_Left[0] != '.') {
		return _Left == domain;
	}

	return domain.rfind(_Left.mid(1)) == 0;
}

bool CookieJar::_isPath(const String & _Left, const String & _Right) const {
	return _Right.find(_Left) == 0;
}