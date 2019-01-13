#pragma once
#include "Utility.hpp"
#include <string>
using std::string;
namespace Cyan
{
	class MLParser;
	class Node;
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
		size_t txtOffset;// "[<]html>"
		size_t count;//the number of characters counted from "[<]html>" to " </html[>] "
		Node *Copy(Node *parent_)
		{
			Node *tNode = new Node(parent_);
			tNode->tagName = tagName;
			tNode->attributes = attributes;
			tNode->txtOffset;
			tNode->count;
			tNode->brother = brother->Copy(parent_);
			tNode->child = child->Copy(tNode);
			return tNode;
		}
		Node *CreateChild()
		{
			if (child == nullptr)
			{
				child = new Node(this);
				return child;
			}
			else
			{
				Node *tNode = child;
				while (tNode->brother != nullptr)
				{
					tNode = tNode->brother;
				}
				tNode->brother = new Node(this);
				return tNode->brother;
			}
		}
		void AddChild(Node *Child_)
		{
			if (child == nullptr)
			{
				child = Child_;
			}
			else
			{
				Node *tNode = child;
				while (tNode->brother != nullptr)
				{
					tNode = tNode->brother;
				}
				tNode->brother = Child_;
			}
		}
		void MoveTo(Node *dst)
		{
			parent->child = nullptr;//�ҵĸ��ڵ㻹��Ϊ���������ӽڵ㣬������
			dst->AddChild(this);//��dstָ���Լ�
			this->parent = dst;//���Լ�ָ��dst
			//brother�ڵ���ͨ�����ҡ�����ڵ���ʵģ��Ͳ���Ҫ�ظ�move��ȥ�ˡ�
			Node *tNode = brother;
			while (tNode != nullptr)
			{
				tNode->parent = dst;
				tNode = tNode->brother;
			}
		}
		Attribute *AddAttribute(string name, string value)
		{
			if (attributes == nullptr)
			{
				attributes = new Attribute(name, value);
				return attributes;
			}
			else
			{
				Attribute *tA = attributes;
				while (tA->next != nullptr)
				{
					tA = tA->next;
				}
				tA->next = new Attribute(name, value);
				return tA->next;
			}
		}
	public:
		Node()
			:parent(nullptr),
			brother(nullptr),
			child(nullptr),
			tagName(""),
			attributes(nullptr),
			txtOffset(0),
			count(0) {}
		Node(Node *parent_)
			:parent(parent_), 
			brother(nullptr), 
			child(nullptr),
			tagName(""), 
			attributes(nullptr), 
			txtOffset(0),
			count(0) {}
		~Node() { delete attributes; delete brother; delete child; }
		string *GetAttribute(const string & AttributeName) const//��Ҫ�޸�
		{
			Attribute *tA = attributes;
			while (tA != nullptr)
			{
				if (tA->name == AttributeName)
				{
					return &tA->value;
				}
				tA = tA->next;
			}
			return nullptr;
		}
	};
}