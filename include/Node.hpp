#pragma once
#include <string>
using std::string;
namespace Cyan
{
	class MLParser;
	class Attribute
	{
		friend class Node;
		friend class MLParser;
	private:
		string name;
		string value;
		Attribute *next;
	public:
		Attribute(string name_, string value_) :name(name_), value(value_), next(nullptr) {};
		~Attribute() { delete next; }
	};
	class Node
	{
		friend class MLParser;
	private:
		Node *parent;
		Node *brother;
		Node *child;
		string tagName;
		Attribute *attributes;
		const char *start;//saving a pointer to the start of tag "[<]html>"
		int count;//the number of characters counted from "[<]html>" to " </html[>] "
	public:
		Node(Node *parent_, string &tagName_)
			:parent(parent_), 
			brother(nullptr), 
			child(nullptr),
			tagName(tagName_), 
			attributes(nullptr), 
			start(nullptr),
			count(0) {};
		~Node() { delete attributes; delete brother; delete child; }
		string GetAttribute(const string & AttributeName) const
		{
			Attribute *tA = attributes;
			while (tA != nullptr)
			{
				if (tA->name == AttributeName)
				{
					return tA->value;
				}
				tA = tA->next;
			}
			return nullptr;
		}
	};
}