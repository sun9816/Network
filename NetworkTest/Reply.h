#pragma once
#include "Url.h"

namespace Network {
	class Reply {
	public:
		Reply();
		Reply(const Reply & _Rep);
		virtual ~Reply();

	public:
		int error() const;

		String rawHeader(const String & _Name) const;
		StringList rawAllHeader() const;

		Url url() const;
		String body() const;

		Reply & operator==(const Reply & _Rep);

	private:
		void setError(int _Error);
		void setUrl(const Url & _Url);
		void setHeader(const String & _Name, const String & _Value);

	private:
		int		m_nError;
		Url		m_Url;
		String	m_strBody;
		std::map<String, String>		m_mapHeader;
	};
}

