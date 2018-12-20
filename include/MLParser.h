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
		void Copy(MLParser & MLP)
		{
			if (MLP.raw == this->raw) return;
			this->Dispose();

			this->raw = Cyan::strcpy(MLP.raw);

			root = MLP.root->Copy(nullptr);
			now = root;
		}
	public:
		MLParser() :raw(nullptr), root(nullptr), now(nullptr) {}
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
		MLParser & operator[](string tagName);
		MLParser & operator[](int n);
		MLParser & XPath(string xpath);
		string GetTagName() const;
		string GetAttribute(const string & AttributeName) const;
		string GetContent() const;//return content removed tags
		string GetInner() const;//return raw inner text
		void Dispose()
		{
			delete[] raw;
			delete root;
			delete now;
		}
	};
}