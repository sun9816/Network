#include "Cookie.h"

using namespace std;
using namespace Network;

Cookie::Cookie() {
}

Cookie::~Cookie() {
}

bool Cookie::setRawString(const String & _Str, const Url & _Url) {
	_setDefault();

	StringList list = _Str.split(";");
	if (list.empty()) {
		return false;
	}
	StringList kvlist = list[0].split("=", 2);
	if (kvlist.size() != 2) {
		return false;
	}
	m_strName = kvlist[0].trim();
	m_strValue = kvlist[1].trim();
	if (m_strName.empty()
		|| m_strValue.empty()) {
		return false;
	}

	for (size_t i = 1; i < list.size(); ++i) {
		kvlist = list[i].split("=", 2);
		switch (kvlist.size()) {
		case 0:
			break;
		case 1:
			if (kvlist[0].lower() == "secure") {
				m_bSecure = true;
			}
			else if (kvlist[0].lower() == "httponly") {
				m_bHttpOnly = true;
			}
			break;
		case 2:
			if (kvlist[0].lower() == "domail") {
				m_strDomail = kvlist[1];
			}
			else if (kvlist[0].lower() == "path") {
				m_strPath = kvlist[1];
			}
			else if (kvlist[0].lower() == "max-age") {

			}
			else if (kvlist[0].lower() == "expires") {
				
			}
			break;
		default:
			break;
		}
	}

	return true;
}

void Cookie::_setDefault() {
	m_strName.clear();
	m_strValue.clear();
	m_strDomail.clear();
	m_strPath.clear();
	m_timeExpires = 0;
	m_bHttpOnly = false;
	m_bSecure = false;
}