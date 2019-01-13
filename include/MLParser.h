#pragma once
#include "Node.hpp"
#include <string>
#include <map>
//#include <gtest\gtest.h>
using std::multimap;
using std::string;
using std::make_pair;
namespace Cyan
{
	typedef multimap<string, Node *> StrNode;
	class Result
	{
	friend class MLParser;
	private:
		char *raw;
		Node *node;
		Result(char *raw_, Node *node_) :raw(raw_),node(node_) {}
	public:
		string GetTagName()
		{
			return node->tagName;
		}
		bool FindAttribute(const string & AttributeName, string & AttributeValue)
		{
			string *ps = node->GetAttribute(AttributeName);
			if (ps == nullptr)
			{
				AttributeValue = "";
				return false;
			}
			AttributeValue = string(*ps);
			return true;
		}
		string GetContent() const;
		string GetInner()
		{
			return substr(raw + node->txtOffset, node->count);
		}
		~Result() {}
	};
	class MLParser
	{
	private:
		char *raw;//saving raw text
		Node *root;
		Node *now;//saving a location for query
		string errorMsg;
		StrNode TagName;//存储 <tagname,Node*> 元组，用于模糊查询
		void Print(Cyan::Node * node, size_t count, bool printAttribute, bool printBrother);
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
		void AddTag(Node *node)
		{
			TagName.insert(make_pair(node->tagName, node));
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
		void PrintTree(bool printAttributes = false)
		{
			Print(now, 0, printAttributes, false);
		}
		MLParser & operator[](string tagName)
		{
			if (tagName == "") { now = root; return *this; }
			if (now->child != nullptr)
			{
				if (now->child->tagName == tagName)
				{
					now = now->child;
					return *this;
				}
				Node *tNode = now->child->brother;
				while (tNode != nullptr)
				{
					if (tNode->tagName == tagName)
					{
						now = tNode;
						return *this;
					}
					tNode = tNode->brother;
				}
			}
			SetErrMsg("Can't find <" + tagName + ">");
			return *this;
		}
		MLParser & operator[](unsigned short n)
		{
			if (n == 0) { return *this; }
			unsigned short i = 0;
			Node *tNode = now->brother;
			while (tNode != nullptr)
			{
				if (tNode->tagName == now->tagName)
				{
					++i;
				}
				if (i == n)
				{
					now = tNode;
					return *this;
				}
				tNode = tNode->brother;
			}
			SetErrMsg("Can't find <" + now->tagName + ">[" + std::to_string(n) + "]");
			return *this;
		}
		MLParser & XPath(string xpath);
		string GetTagName() 
		{
			string t = now->tagName;
			now = root;
			return t;
		}
		bool FindAttribute(const string & AttributeName,string & AttributeValue) 
		{
			string *ps = now->GetAttribute(AttributeName);
			if (ps == nullptr)
			{
				SetErrMsg("Can't find Attribute '" + AttributeName + "'");
				AttributeValue = "";
				return false;
			}
			AttributeValue = string(*ps);
			now = root;
			return true;
		}
		string GetContent() const;
		string GetInner()
		{
			string t =  substr(raw + now->txtOffset, now->count);
			now = root;
			return t;
		}
		string GetErrorMsg() const
		{
			return errorMsg;
		}
		void SetErrMsg(string msg)
		{
			errorMsg = msg;
		}
		void Dispose()
		{
			delete[] raw;
			delete root;
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