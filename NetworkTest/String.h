#pragma once
#include <vector>
#include <string>
namespace Network {
	class StringList;
	class String : public std::vector<char> {
	public:
		static const size_t	npos = -1;

	public:
		String();
		String(char _Ch);
		String(size_t _Size, char _Ch = 0);
		String(const char *_Str);
		String(const char *_Str, size_t _Size);
		String(const std::string & _Other);
		String(const String & _Other);
		String(String && _Other);
		String(const_iterator _First, const_iterator _Last);
		String(const_reverse_iterator _First, const_reverse_iterator _Last);
		virtual ~String();
		
	public:
		String & append(const String & _Str);
		String & append(const char _Ch, size_t _Size);
		String & append(const char * _Str, size_t _Size);
		String & append(char _Ch);

		String & format(const char * _Format, ...);
		String & format_append(const char * _Format, ...);

		size_t find(const String & _Str, size_t _Off = 0) const;
		
		StringList spilt(const String & _Sep, size_t _MaxCount = -1) const;

		std::string stdstr() const;
		
		String left(size_t _Size) const;
		String right(size_t _Size) const;
		String mid(size_t _Off, size_t _Size) const;
		String substr(const String & _Left, const String & _Right, size_t _Off = 0) const;

		String upper() const;
		String lower() const;
		String trim() const;

		int compare(const String & _Str) const;
		
	public:
		String & operator+=(const String & _Str);
		String & operator+=(char _Ch);
		String & operator=(const String & _Str);

		bool operator==(const String & _Str) const;
		bool operator>(const String & _Str) const;
		bool operator<(const String & _Str) const;

	public:
		static String toUTF8(const String & _Str);
		static String fromUTF8(const String & _Str);
	};

	class StringList : public std::vector<String> {
	public:
		String join(const String & _Sep = String(""));
	};
}

