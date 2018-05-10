#pragma once
#include "String.h"
#include <map>

#define URL_URL			("Url")
#define URL_SCHEME		("Scheme")
#define URL_SLASH		("Slash")
#define URL_HOST		("Host")
#define URL_PORT		("Port")
#define URL_PATH		("Path")
#define URL_QUERY		("Query")
#define URL_HASH		("Hash")

namespace Network {
	class Url {
	public:
		Url();
		Url(const String & _Str);
		Url(const Url & _Url);

		virtual ~Url();

	public:
		bool setUrl(const String & _Str);
		void setValue(const String & _Name, const String & _Str);

		String value(const String & _Name);

	public:
		static String encodeURI(const String & _Str);
		static String decodeURI(const String & _Str);
		static String encodeURIComponent(const String & _Str);
		static String decodeURIComponent(const String & _Str);

	private:
		static String encode(const String & _Str, const char *_Tables);
		static String decode(const String & _Str, const char *_Tables);

	private:
		std::map<String, String>	m_mapUrl;
	};
}