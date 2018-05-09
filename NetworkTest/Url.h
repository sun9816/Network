#pragma once
#include "String.h"

namespace Network {
	class Url {
	public:
		Url();
		virtual ~Url();

	public:
		static String encodeURI(const String & _Str);
		static String decodeURI(const String & _Str);
		static String encodeURIComponent(const String & _Str);
		static String decodeURIComponent(const String & _Str);

	private:
		static String encode(const String & _Str, const char *_Tables);
		static String decode(const String & _Str);

	};
}