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
	class Result
	{
		friend class MLParser;
	private:
		char *raw;
		Node *node;
		Result(char *raw_, Node *node_) :raw(raw_), node(node_) {}
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
}