#include "MLParser.h"
#include <stack>
#include <regex>
using std::stack;
using std::string;
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
					lastToken->offset = offset;
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
					///<text style="aaa" autofocus></text>
					//The attribute "autofocus" will cause a error
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
	void MLParser::preprocess(string &m)
	{
		std::regex exp1("<script([\\S\\s]+?)<\\/script>");
		m = std::regex_replace(m, exp1, "");
		std::regex exp2("<!--([\\S\\s]+?)-->");
		m = std::regex_replace(m, exp2, "");
		std::regex exp3("<!([\\S\\s]+?)>");
		m = std::regex_replace(m, exp3, "");
		std::regex exp4("<style([\\S\\s]+?)<\\/style>");
		m = std::regex_replace(m, exp4, "");
	}
	bool MLParser::Parse(string html)
	{
		preprocess(html);
		raw = Cyan::strcpy(html.data());//copy data

		size_t i, k;
		for (i = k = 0;raw[i] != '\0'; ++i)
		{
			if (raw[i] == '\n' || raw[i] == '\r' || raw[i] == '\t') {}
			else
			{
				raw[k] = raw[i];
				k = k + 1;
			}
		}
		raw[k] = '\0';

		for (size_t i = 0; i < strlen(raw); i++)
		{
			std::cout << raw[i];
		}

		//initial Scanner & Scan
		Scanner SC = Scanner(raw);
		SC.Scan();

		stack<Node *> NodeStack;
		stack<Node *> t;
		root = new Node();
		Node *lNode = root;//记录上一个操作过的Node
		Node *pNode = root;//下一个新节点的父亲节点
		Node *tNode = root;//便于更新Node信息
		NodeStack.push(root);
		Attribute *lAttribute = nullptr;//记录上一个操作过的Node;

		//****************************************
		//因为设计错误，不得不使用该变量记录一些信息
		//遇到LeftAngleBracket和EndTag会改变这个值（改为L_A_B或者EndTag）
		//遇到RightAngleBracket会使用这个值：
		//如果该变量(tType) == LeftAngleBracket
		//那么RightAngleBracket就修改lNode的txtOffset字段为它自身的offset
		//反之则不做任何操作
		//****************************************
		TokenType tType = Default;

		Token *token = nullptr;
		while ((token = SC.next()) != nullptr)
		{
			switch (token->type)
			{
			case LeftAngleBracket:
				tNode = pNode->CreateChild();//Create a new child node
				tNode->tagName = token->value;
				pNode = lNode = tNode;
				NodeStack.push(tNode);
				t.push(tNode);//用于测试
				tType = LeftAngleBracket;//见tType声明前注释
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
					if(tNode->child != nullptr)
						tNode->child->MoveTo(tNode->parent);

					tNode = NodeStack.top();
					NodeStack.pop();
				}
				tNode->count = token->offset - tNode->txtOffset;
				pNode = NodeStack.top();
				tType = EndTag;//见tType声明前注释
				break;
			case RightAngleBracket:
				if (tType == LeftAngleBracket)//见tType变量声明的注释
				{
					lNode->txtOffset = token->offset + 1;//token->offset指向右尖括号，+1改为指向内容的第一个字符
				}
				break;
			default: 
				break;
			}
		}
		now = root;
		Debug_Show(root, 0);

		return true;
	}
	void MLParser::Debug_Show(Cyan::Node *node, size_t count)
	{
		using std::cout;
		using std::endl;
		if (node != nullptr)
		{
			for (size_t i = 0; i < count; ++i)
			{
				cout << "·";
			}
			cout << "<" << node->tagName;
			Attribute *tA = node->attributes;
			while (tA != nullptr)
			{
				cout << ' ' << tA->name << "=\"" << tA->value << '"';
				tA = tA->next;
			}
			cout << ">" << endl;
			if (node->child != nullptr)
			{
				Debug_Show(node->child, count + 1);
			}
			for (size_t i = 0; i < count; ++i)
			{
				cout << "·";
			}
			cout << "</" << node->tagName << ">" << endl;
			if (node->brother != nullptr)
			{
				Debug_Show(node->brother, count);
			}
		}
	}
}