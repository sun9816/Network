#include "Reply.h"

using namespace std;
using namespace Network;

Reply::Reply() {
}

Reply::Reply(const Reply & _Rep):
	m_nError(_Rep.m_nError),
	m_Url(_Rep.m_Url),
	m_strBody(_Rep.m_strBody),
	m_mapHeader(_Rep.m_mapHeader) {
}

Reply::~Reply() {
}

int Reply::error() const {
	return m_nError;
}

String Reply::rawHeader(const String & _Name) const {
	auto it = m_mapHeader.find(_Name);
	if (it == m_mapHeader.end()) {
		return String("");
	}
	return it->second;
}

StringList Reply::rawAllHeader() const {
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

Url Reply::url() const {
	return m_Url;
}

String Reply::body() const {
	return m_strBody;
}

Reply & Reply::operator==(const Reply & _Rep) {
	m_nError = _Rep.m_nError;
	m_Url = _Rep.m_Url;
	m_strBody = _Rep.m_strBody;
	m_mapHeader = _Rep.m_mapHeader;
}

void Reply::setUrl(const Url & _Url) {
	m_Url = _Url;
}

void Reply::setHeader(const String & _Name, const String & _Value) {
	m_mapHeader[_Name.trim()] = _Value.trim();
}
