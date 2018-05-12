#pragma once
#include "Cookie.h"
#include <vector>

namespace Network {
	class CookieJar {
	public:
		CookieJar();
		CookieJar(const CookieJar & _Cookiejar);
		virtual ~CookieJar();

	public:
		CookieList cookieForUrl(const Url & _Url) const;

		bool updateCookie(const Cookie & _Cookie);
		bool deleteCookie(const Cookie & _Cookie);
		bool insertCookie(const Cookie & _Cookie);

		CookieJar & operator=(const CookieJar & _Cookiejar);

	private:
		bool _isDomain(const String & _Left, const String & _Right) const;
		bool _isPath(const String & _Left, const String & _Right) const;

	private:
		std::vector<Cookie>	m_vecCookieJar;
	};
}

