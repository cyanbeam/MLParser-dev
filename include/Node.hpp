#pragma once
#include <string>
#include "Utility.hpp"
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
		Attribute(string name_, string value_) :name(name_), value(value_), next(nullptr) {}
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
		int txtOffset;// "[<]html>"
		int count;//the number of characters counted from "[<]html>" to " </html[>] "
	public:
		Node(Node *parent_)
			:parent(parent_), 
			brother(nullptr), 
			child(nullptr),
			tagName(nullptr), 
			attributes(nullptr), 
			txtOffset(0),
			count(0) {}
		Node *Copy(Node *parent_)
		{
			Node *tNode = new Node(parent_);
			tNode->txtOffset;
			tNode->count;
			tNode->brother = brother->Copy(parent_);
			tNode->child = child->Copy(tNode);
			return tNode;
		}
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