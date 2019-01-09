#include "MLParser.h"
#include <stack>
using std::stack;
namespace Cyan
{
	void Scanner::Scan()
	{
		size_t offset = 0;
		Token *lastToken = root;
		size_t len = strlen(raw);
		while (true)
		{
			if (offset >= len) break;
			if (raw[offset] == '<')
			{
				if (raw[offset + 1] == '/')
				{
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = EndTag;
					offset += 2;//使得raw[offset]=tagName的第一位字符
					lastToken->value = gstrExcept('>', offset);
					continue;
				}
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = LeftAngleBracket;
				lastToken->offset = offset;
				offset += 1;//使得raw[offset]=tagName的第一位字符
				lastToken->value = gstrExcept(' ', '>', offset);
				while (raw[offset] != '>')
				{
					skipAll(' ', offset);
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeName;
					lastToken->value = gstrExcept('=', ' ', '>', offset);
					skipAll(' ', offset);
					skipAll('=', '"', '\'', offset);

					//html code:
					///<text style="aaa" auto></text>
					//The attribute "auto" will cause a error
					//
					//the next judge is to fix this bug.
					if (raw[offset] == '>') break;

					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeValue;
					lastToken->value = gstrExcept('"', '\'', ' ', '>', offset);
					skipAll('"', '\'', ' ', offset);
				}
			}
			if (raw[offset] == '>')
			{
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = RightAngleBracket;
				lastToken->offset = offset;
			}
			++offset;
		}
		now = root;
	}
	bool MLParser::Parse(string html)
	{
		raw = Cyan::strcpy(html.data());//copy data

		//initial Scanner & Scan
		Scanner SC = Scanner(raw);
		SC.Scan();

		stack<Node *> NodeStack;
		root = new Node();
		Node *lNode = root;//记录上一个操作过的Node
		Node *pNode = root;//下一个新节点的父亲节点
		Node *tNode = root;//便于更新Node信息
		NodeStack.push(root);
		Attribute *lAttribute = nullptr;//记录上一个操作过的Node;

		Token *token = nullptr;
		while ((token = SC.next()) != nullptr)
		{
			switch (token->type)
			{
			case LeftAngleBracket:
				if (pNode->child == nullptr)
				{
					tNode = pNode->child = new Node(pNode);
				}
				else
				{
					tNode = pNode->child;
					while (tNode->brother != nullptr)
					{
						tNode = tNode->brother;
					}
					tNode = tNode->brother = new Node(pNode);
				}
				tNode->tagName = token->value;
				pNode = lNode = tNode;
				NodeStack.push(tNode);
				break;
			case AttributeName:
				if (lNode->attributes == nullptr)
				{
					lAttribute =  lNode->attributes = new Attribute(token->value, "");
				}
				else
				{
					lAttribute = lNode->attributes;
					while (lAttribute->next != nullptr)
					{
						lAttribute = lAttribute->next;
					}
					lAttribute = lAttribute->next = new Attribute(token->value, "");
				}
				break;
			case AttributeValue:
				lAttribute->value = token->value;
				break;
			case EndTag:
				if (NodeStack.size() <= 0)break;
				tNode = NodeStack.top();
				NodeStack.pop();
				while (tNode->tagName != token->value)
				{
					//下面的代码用于修复parent的指向错误
					tNode->child->parent = tNode->parent;
					tNode = tNode->brother;
					while (tNode != nullptr)
					{
						tNode->parent = tNode->parent->parent;
						tNode = tNode->brother;
					}

					tNode = NodeStack.top();
					NodeStack.pop();
				}
				pNode = NodeStack.top();
				break;
			case RightAngleBracket:
				std::cout << ">" << std::endl;
				break;
			default: 
				break;
			}
		}
		now = root;
		return true;
	}
}