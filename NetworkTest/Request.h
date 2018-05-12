#pragma once
#include "Url.h"

namespace Network {
	class Request {
	public:
		Request();
		Request(const Request & _Req);
		virtual ~Request();

	public:
		void setUrl(const Url & _Url);
		void setHeader(const String & _Name, const String & _Value);

		Url url() const;
		StringList rawAllHeader() const;
		String rawHeader(const String & _Name) const;

		Request & operator=(const Request & _Req);

	private:
		Url		m_Url;
		map<String, String>	m_mapHeader;
	};
}
