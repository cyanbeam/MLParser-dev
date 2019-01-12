#pragma once
#include "Node.hpp"
#include <string>
//#include <gtest\gtest.h>
using std::string;
namespace Cyan
{
	class MLParser
	{
	private:
		char *raw;//saving raw text
		Node *root;
		Node *now;//saving a location for query
		string errorMsg;
		void preprocess(string &m);
		void Copy(MLParser & MLP)
		{
			if (MLP.raw == this->raw) return;
			this->Dispose();

			this->raw = Cyan::strcpy(MLP.raw);

			this->root = MLP.root->Copy(nullptr);
			this->now = root;
			this->errorMsg = nullptr;
		}
	public:
		MLParser() :raw(nullptr), root(nullptr), now(nullptr), errorMsg("") {}
		MLParser(MLParser & MLP)
		{
			Copy(MLP);
		}
		MLParser & operator=(MLParser & MLP)
		{
			Copy(MLP);
			return *this;
		}
		~MLParser() {}
		bool Parse(string html);
		void Print(Cyan::Node * node, size_t count, bool printAttribute);
		MLParser & operator[](string tagName);
		MLParser & operator[](int n);
		MLParser & XPath(string xpath);
		string GetTagName() const;
		string GetAttribute(const string & AttributeName) const;
		string GetContent() const;//return content removed tags
		string GetInner() const;//return raw inner text
		string GetErrorMsg() const
		{
			return errorMsg;
		}
		void Dispose()
		{
			delete[] raw;
			delete root;
			delete now;
		}
	};
	enum TokenType
	{
		Default,
		LeftAngleBracket,
		EndTag,
		AttributeName,
		AttributeValue,
		RightAngleBracket
	};
	struct Token
	{
		TokenType type = Default;
		size_t offset = 0;
		string value = "";
		Token *next = nullptr;
		~Token()
		{
			delete next;
		}
	};
	class Scanner
	{
//#ifdef _DEBUG
//		FRIEND_TEST(Scanner_Test, GetAttributeValue_Test);
//#endif
	private:
		const char *raw;
		Token *root;
		Token *now;
		void skipExcept(char exception, size_t &offset)
		{
			while (raw[offset] != exception && raw[offset] != '\0') ++offset;
		}
		void skipExcept(char exceptionA, char exceptionB, size_t &offset)
		{
			while (raw[offset] != exceptionA && raw[offset] != exceptionB && raw[offset] != '\0') ++offset;
		}
		void skipExcept(char exceptionA, char exceptionB, char exceptionC, size_t &offset)
		{
			while (raw[offset] != exceptionA && raw[offset] != exceptionB && raw[offset] != exceptionC && raw[offset] != '\0') ++offset;
		}
		void skipExcept(char exceptionA, char exceptionB, char exceptionC, char exceptionD, size_t &offset)
		{
			while (raw[offset] != exceptionA && raw[offset] != exceptionB && raw[offset] != exceptionC && raw[offset] != exceptionD && raw[offset] != '\0') ++offset;
		}
		void skipAll(char exception, size_t &offset)
		{
			while (raw[offset] == exception && raw[offset] != '\0') ++offset;
		}
		void skipAll(char exceptionA, char exceptionB, size_t &offset)
		{
			while ((raw[offset] == exceptionA || raw[offset] == exceptionB)&& raw[offset] != '\0') ++offset;
		}
		void skipAll(char exceptionA, char exceptionB, char exceptionC, size_t &offset)
		{
			while ((raw[offset] == exceptionA || raw[offset] == exceptionB || raw[offset] == exceptionC) && raw[offset] != '\0') ++offset;
		}
		string gstrExcept(char exception, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exception,offset);
			count = offset - a;
			return substr(raw + a, count);
		}
		string gstrExcept(char exceptionA, char exceptionB, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exceptionA, exceptionB, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
		string gstrExcept(char exceptionA, char exceptionB, char exceptionC, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exceptionA, exceptionB,exceptionC, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
		string gstrExcept(char exceptionA, char exceptionB, char exceptionC, char exceptionD, size_t &offset)
		{
			size_t a = offset, count = 0;
			skipExcept(exceptionA, exceptionB, exceptionC, exceptionD, offset);
			count = offset - a;
			return substr(raw + a, count);
		}
	public:
		Scanner(const char *content) :raw(content), root(nullptr), now(nullptr)
		{
			root = new Token;
		}
		~Scanner() { delete root; }
		void Scan();
		Token *next()
		{
			now = now->next;
			return now;
		}

	};
}