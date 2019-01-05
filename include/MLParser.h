#pragma once
#include <string>
#include "Node.hpp"
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
		MLParser() :raw(nullptr), root(nullptr), now(nullptr), errorMsg(nullptr) {}
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
		bool Parse(string & html);
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
	private:
		const char *raw;
		Token *root;
		Token *now;
		string GetTagName(size_t &offset)
		{
			size_t a = offset, count = 0;
			while (raw[offset] != '>' && raw[offset] != ' ' && raw[offset] != '\0') ++offset;
			count = offset - a;
			return substr(raw + a, count);
		}
		string GetAttributeName(size_t &offset)
		{
			while (raw[offset] == ' ' && raw[offset] != '\0') ++offset;
			size_t a = offset, count = 0;
			while (raw[offset] != '=' && raw[offset] != '>' && raw[offset] != ' ' && raw[offset] != '\0') ++offset;
			count = offset - a;
			return substr(raw + a, count);
		}
		string GetAttributeValue(size_t &offset)
		{
			if (raw[offset] == '>') return string("");//<textarea auto></textarse>,Attribute "auto" have no value
			while (raw[offset] != '=' && raw[offset] != '\0') ++offset;
			while ((raw[offset] == '=' || raw[offset] == ' ') && raw[offset] != '\0') ++offset;
			while ((raw[offset] == '"' || raw[offset] == '\'') && raw[offset] != '\0') ++offset;
			size_t a = offset, count = 0;
			char ss = raw[offset - 1];//it should be ["] or [']
			if (ss != '"' && ss != '\'')
			{
				while (raw[offset] != ' ' && raw[offset] != '>' && raw[offset] != '\0') ++offset;
			}
			else
			{
				while (raw[offset] != '\0')
				{
					if (raw[offset] == ss && raw[offset - 1] != '\\') break;
					++offset;
				}
			}
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