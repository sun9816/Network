#pragma once
#include "String.h"
#include "Url.h"
#include <functional>
#include <map>

namespace Network {
	class StringList;
	class Cookie {
	public:
		Cookie();
		Cookie(const String & _Str, const Url & _Url);
		Cookie(const Cookie & _Cookie);
		virtual ~Cookie();

	public:
		bool setRawString(const String & _Str, const Url & _Url);

		String name() const;
		String value() const;
		String domain() const;
		String path() const;
		time_t expires() const;
		bool isHttpOnly() const;
		bool isSecure() const;

		bool operator==(const Cookie & _Cookie) const;
		bool operator!=(const Cookie & _Cookie) const;
		Cookie & operator=(const Cookie & _Cookie);

	private:
		void _setDefault();
		void _initMap();
		void _setGMTtime(const String & _Str);
	private:
		String		m_strName;
		String		m_strValue;
		String		m_strDomain;
		String		m_strPath;
		time_t		m_timeExpires;
		bool		m_bHttpOnly;
		bool		m_bSecure;

		std::map<String, std::function<void(const String &)>> m_mapKv;
	};

	class CookieList : public std::vector<Cookie> {
	public:
		String join(const String & _Sep = String(";")) const;
	};
}

