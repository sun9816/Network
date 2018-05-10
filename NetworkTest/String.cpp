#include "String.h"
#include <algorithm>
#include <cstdarg>
#include <memory>
#include <Windows.h>

using namespace std;
using namespace Network;

String::String() {
}

String::String(char _Ch) :
	vector(_Ch) {
}

String::String(size_t _Size, char _Ch) :
	vector(_Size, _Ch) {
}

String::String(const char * _Str) {
	vector::assign(strlen(_Str), 0);
	transform(vector::begin(), vector::end(), vector::begin(), [&](char) {
		return *_Str++;
	});
}

String::String(const char * _Str, size_t _Size) {
	vector::assign(_Size, 0);
	transform(vector::begin(), vector::end(), vector::begin(), [&](char) {
		return *_Str++;
	});
}

String::String(const string & _Other) :
	vector(_Other.size()) {
	memcpy(vector::data(), _Other.data(), _Other.size());
}

String::String(const String & _Other) :
	vector(_Other) {
}

String::String(String && _Other) :
	vector(move(_Other)) {
}

String::String(const_iterator _First, const_iterator _Last) :
	vector(_First, _Last) {
}

String::String(const_reverse_iterator _First, const_reverse_iterator _Last) :
	vector(_First, _Last) {
}

String::~String() {
}

String & String::append(const String & _Str) {
	for (auto & it : _Str) {
		vector::push_back(it);
	}
	return *this;
}

String & String::append(const char _Ch, size_t _Size) {
	for (size_t i = 0; i < _Size; ++i) {
		vector::push_back(_Ch);
	}
	return *this;
}

String & String::append(const char * _Str, size_t _Size) {
	for (size_t i = 0; i < _Size; ++i) {
		vector::push_back(*_Str++);
	}
	return *this;
}

String & String::append(char _Ch) {
	vector::push_back(_Ch);
	return *this;
}

String & String::format(const char * _Format, ...) {
	va_list arg;
	va_start(arg, _Format);

	int bufferLen = _vscprintf_p(_Format, arg) + 1;
	shared_ptr<char> buffer(new(nothrow) char[bufferLen]);
	int strLen = _vsprintf_p(buffer.get(), bufferLen, _Format, arg);

	va_end(arg);

	vector::clear();
	return append(buffer.get(), strLen);
}

String & String::format_append(const char * _Format, ...) {
	va_list arg;
	va_start(arg, _Format);

	int bufferLen = _vscprintf_p(_Format, arg) + 1;
	shared_ptr<char> buffer(new(nothrow) char[bufferLen]);
	int strLen = _vsprintf_p(buffer.get(), bufferLen, _Format, arg);

	va_end(arg);

	return append(buffer.get(), strLen);
}

size_t String::find(const String & _Str, size_t _Off) const {
	if (vector::size() > _Off) {
		for (size_t i = _Off; i < vector::size(); ++i) {
			bool isfind = true;
			for (size_t n = 0; n < _Str.size(); ++n) {
				if (vector::operator[](i + n) != _Str[n]) {
					isfind = false;
					break;
				}
			}

			if (isfind) {
				return i;
			}
		}

	}
	return npos;
}

StringList String::split(const String & _Sep, size_t _MaxCount) const {
	size_t pos = 0, offset = 0;
	StringList list;

	while ((pos = find(_Sep, offset)) != npos && _MaxCount > 1) {
		list.push_back(move(String(vector::data() + offset, pos - offset)));
		offset = pos + _Sep.size();
		--_MaxCount;
	}
	if (offset <= vector::size() && _MaxCount > 0) {
		list.push_back(move(String(vector::data() + offset, vector::size() - offset)));
	}

	return list;
}

string String::stdstr() const {
	return string(vector::begin(), vector::end());
}

String String::left(size_t _Size) const {
	if (_Size == npos) {
		return String("");
	}
	if (vector::size() <= _Size) {
		return *this;
	}
	return String(vector::begin(), vector::begin() + _Size);
}

String String::right(size_t _Size) const {
	if (_Size == npos) {
		return String("");
	}
	if (vector::size() <= _Size) {
		return *this;
	}
	return String(vector::end() - _Size, vector::end());
}

String String::mid(size_t _Off, size_t _Size) const {
	if (_Off == npos) {
		return String("");
	}
	if (vector::size() <= _Off) {
		return String("");
	}
	if (_Size == npos
		|| vector::size() <= _Size + _Off) {
		return String(vector::begin() + _Off, vector::end());
	}
	return String(vector::begin() + _Off, vector::begin() + _Off + _Size);
}

String String::substr(const String & _Left, const String & _Right, size_t _Off) const {
	size_t nBegPos = find(_Left, _Off);
	if (nBegPos == string::npos) {
		return String("");
	}
	nBegPos += _Left.size();
	size_t nEndPos = find(_Right, nBegPos);
	if (nEndPos == string::npos) {
		return String("");
	}
	return mid(nBegPos, nEndPos - nBegPos);
}

String String::upper() const {
	String ret(*this);
	transform(ret.begin(), ret.end(), ret.begin(), toupper);
	return ret;
}

String String::lower() const {
	String ret(*this);
	transform(ret.begin(), ret.end(), ret.begin(), tolower);
	return ret;
}

String String::trim() const {
	String::const_iterator first = find_if(vector::begin(), vector::end(), [=](const char _Ch) {return _Ch != ' '; });
	String::const_reverse_iterator last = find_if(vector::rbegin(), vector::rend(), [=](const char _Ch) {return _Ch != ' '; });
	if (first == vector::end()
		|| last == vector::rend()) {
		return String("");
	}

	return String(first, last.base());
}

int String::compare(const String & _Str) const {
	if (vector::size() != _Str.size()) {
		return vector::size() - _Str.size();
	}

	for (size_t i = 0; i < vector::size(); ++i) {
		if (vector::operator[](i) != _Str[i]) {
			return vector::operator[](i) - _Str[i];
		}
	}

	return 0;
}

String & String::operator+=(const String & _Str) {
	return append(_Str);
}

String & String::operator+=(char _Ch) {
	return append(_Ch);
}

String & String::operator=(const String & _Str) {
	vector::resize(_Str.size());
	memcpy(vector::data(), _Str.data(), _Str.size());
	return *this;
}

bool String::operator==(const String & _Str) const {
	return compare(_Str) == 0;
}

bool String::operator>(const String & _Str) const {
	return compare(_Str) > 0;
}

bool String::operator<(const String & _Str) const {
	return compare(_Str) < 0;
}

String String::toUTF8(const String & _Str) {
	int wideSize = MultiByteToWideChar(CP_ACP, 0, _Str.data(), _Str.size(), nullptr, 0);
	if (wideSize == 0) {
		return String("");
	}

	shared_ptr<wchar_t> wcharBuffer(new(nothrow) wchar_t[wideSize]);
	if (MultiByteToWideChar(CP_ACP, 0, _Str.data(), _Str.size(), wcharBuffer.get(), wideSize) == 0) {
		return String("");
	}

	int multiSize = WideCharToMultiByte(CP_UTF8, 0, wcharBuffer.get(), wideSize, nullptr, 0, nullptr, nullptr);
	if (multiSize == 0) {
		return String("");
	}

	String multiBuffer(multiSize, 0);
	if (WideCharToMultiByte(CP_UTF8, 0, wcharBuffer.get(), wideSize, const_cast<char *>(multiBuffer.data()), multiBuffer.size(), nullptr, nullptr) == 0) {
		return String("");
	}

	return multiBuffer;
}

String String::fromUTF8(const String & _Str) {
	int wideSize = MultiByteToWideChar(CP_UTF8, 0, _Str.data(), _Str.size(), nullptr, 0);
	if (wideSize == 0) {
		return String("");
	}

	shared_ptr<wchar_t> wcharBuffer(new(nothrow) wchar_t[wideSize]);
	if (MultiByteToWideChar(CP_UTF8, 0, _Str.data(), _Str.size(), wcharBuffer.get(), wideSize) == 0) {
		return String("");
	}

	int multiSize = WideCharToMultiByte(CP_ACP, 0, wcharBuffer.get(), wideSize, nullptr, 0, nullptr, nullptr);
	if (multiSize == 0) {
		return String("");
	}

	String multiBuffer(multiSize, 0);
	if (WideCharToMultiByte(CP_ACP, 0, wcharBuffer.get(), wideSize, const_cast<char *>(multiBuffer.data()), multiBuffer.size(), nullptr, nullptr) == 0) {
		return String("");
	}

	return multiBuffer;
}

/*StringList*/

String StringList::join(const String & _Sep) {
	String ret;
	bool isfirst = true;
	for (auto & it : *this) {
		if (isfirst) {
			isfirst = false;
		}
		else {
			ret += _Sep;
		}
		ret += it;
	}
	return ret;
}
