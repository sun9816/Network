#pragma once
#include "String.h"
#include <map>

namespace Network {
	class Url {
	public:
		Url();
		Url(const String & _Str);
		Url(const Url & _Url);

		virtual ~Url();

	public:
		bool setUrl(const String & _Str);

		String url() const;
		String scheme() const;
		String host() const;
		String port() const;
		String path() const;
		String query() const;
		String hash() const;

	private:
		void _setDefault();

	public:
		static String encodeURI(const String & _Str);
		static String decodeURI(const String & _Str);
		static String encodeURIComponent(const String & _Str);
		static String decodeURIComponent(const String & _Str);

	private:
		static String encode(const String & _Str, const char *_Tables);
		static String decode(const String & _Str, const char *_Tables);

	private:
		String		m_strUrl;
		String		m_strScheme;
		String		m_strHost;
		String		m_strPort;
		String		m_strPath;
		String		m_strQuery;
		String		m_strHash;
	};
}