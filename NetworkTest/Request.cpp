#include "Request.h"

using namespace std;
using namespace Network;

Request::Request() {
}

Request::Request(const Request & _Req) :
	m_mapHeader(_Req.m_mapHeader) ,
	m_Url(_Req.m_Url) {

}

Request::~Request() {
}

void Request::setUrl(const Url & _Url) {
	m_Url = _Url;
}

void Request::setHeader(const String & _Name, const String & _Value) {
	m_mapHeader[_Name.trim()] = _Value.trim();
}

Url Request::url() const {
	return m_Url;
}

StringList Request::rawAllHeader() const {
	StringList list;
	for (auto & it : m_mapHeader) {
		String str;
		str.append(it.first);
		str.append("= ");
		str.append(it.second);
		list.push_back(move(str));
	}
	return list;
}

String Request::rawHeader(const String & _Name) const {
	auto it = m_mapHeader.find(_Name);
	if (it == m_mapHeader.end()) {
		return String("");
	}
	return it->second;
}

Request & Request::operator=(const Request & _Req) {
	m_mapHeader = _Req.m_mapHeader;
	m_Url = _Req.m_Url;
	return *this;
}