#pragma once
#include "String.h"
#include "Url.h"

namespace Network {
	class Cookie {
	public:
		Cookie();
		virtual ~Cookie();

	public:
		bool setRawString(const String & _Str, const Url & _Url);

	private:
		void _setDefault();

	private:
		String		m_strName;
		String		m_strValue;
		String		m_strDomail;
		String		m_strPath;
		__time64_t	m_timeExpires;
		bool		m_bHttpOnly;
		bool		m_bSecure;
	};
}

