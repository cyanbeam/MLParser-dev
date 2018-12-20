#pragma once
#include <string>
#include "Node.hpp"
using std::string;
namespace Cyan
{
	class MLParser
	{
	private:
		const char *raw;//saving raw text
		Node *root;
		Node *now;//saving a location for query
	public:
		MLParser();
		MLParser(MLParser & MLP);
		MLParser & operator=(MLParser & MLP);
		~MLParser();
		bool Parse(string html);
		MLParser & operator[](string tagName);
		MLParser & operator[](int n);
		MLParser & XPath(string xpath);
		string GetTagName() const;
		string GetAttribute(const string & AttributeName) const;
		string GetContent() const;//return content removed tags
		string GetInner() const;//return raw inner text
		void Dispose();
	};
}