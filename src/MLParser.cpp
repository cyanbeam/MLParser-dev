#include "MLParser.h"
#include <stack>
#include <regex>
#include <list>
using std::list;
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
					str2lwr(lastToken->value);
					continue;
				}
				lastToken->next = new Token;
				lastToken = lastToken->next;
				lastToken->type = LeftAngleBracket;
				lastToken->offset = offset;
				offset += 1;//使得raw[offset]=tagName的第一位字符
				lastToken->value = gstrExcept(' ', '>', offset);
				str2lwr(lastToken->value);
				while (raw[offset] != '>')
				{
					skipAll(' ', offset);
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeName;
					lastToken->value = gstrExcept('=', ' ', '>', offset);
					skipAll(' ','=', offset);
					//skipAll('=', offset);

					//用单、双引号括起来的字符串里可能出现空格，为了正确分割
					//应该再再次遇到单、双引号的时候停止，而不是一刀切的在 空格、单双引号 处直接停止
					//也就是实现 将字符串视为一个整体来分割，即使字符串内包含空格（或单双引号）
					char flag = raw[offset];//当前字符是等号(=)右边的第一个非空格字符  name = ["]value"
					if (flag == '"' || flag == '\'') ++offset;//如果现在是引号，那么跳过，指向内容区
					//html code:
					///<text style="aaa" autofocus></text>
					//The attribute "autofocus" will cause a error
					//
					//the next judge is to fix this bug.
					if (flag == '>') break;
					
					lastToken->next = new Token;
					lastToken = lastToken->next;
					lastToken->type = AttributeValue;
					if(flag == '"' || flag == '\'') 
						lastToken->value = gstrExcept(flag, '>', offset);
					else
						lastToken->value = gstrExcept(' ', '>', offset);
					skipAll('"', '\'', ' ', offset);
					skipAll('/', offset);
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


		//initial Scanner & Scan
		Scanner SC = Scanner(raw);
		SC.Scan();

		stack<Node *> NodeStack;
		list<Node *> NodeList;
		root = new Node();
		root->tagName = "_root_";
		Node *lNode = root;//记录上一个操作过的Node
		Node *pNode = root;//下一个新节点的父亲节点
		Node *tNode = root;//便于更新Node信息
		NodeStack.push(root);
		Attribute *lAttribute = nullptr;//记录上一个操作过的Attribute;

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
				NodeList.push_back(tNode);
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
				if (NodeStack.size() <= 1)break;
				tNode = NodeStack.top();
				NodeStack.pop();
				NodeList.pop_back();
				while (tNode->tagName != token->value)
				{
					list<Node *>::iterator it;
					for (it = NodeList.begin(); it != NodeList.end(); ++it)
					{
						if ((*it)->tagName == token->value) break;
					}
					if (it == NodeList.end())//如果说tNode->tagName != token->value，就不会出现it == NodeList.end()
					{
						//说明这是凭空出现的一个End_Tag
						//这个凭空出现的End_Tag不应该造成任何影响
						//所以我们把弹出来的Node再加回去
						NodeStack.push(tNode);
						NodeList.push_back(tNode);
						break;
					}
					//下面的代码用于修复parent的指向错误
					if(tNode->child != nullptr)
						tNode->child->MoveTo(tNode->parent);

					if (NodeStack.size() <= 1) 
						break;
					tNode = NodeStack.top();
					NodeStack.pop();
					NodeList.pop_back();
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
		return true;
	}
	void MLParser::Print(Cyan::Node *node, size_t count,bool printAttribute,bool printBrother)
	{
		using std::cout;
		using std::endl;
		if (node != nullptr)
		{
			for (size_t i = 0; i < count; ++i)
			{
				cout << "·";
			}
			cout << '<' << node->tagName;
			if (printAttribute)
			{
				Attribute *tA = node->attributes;
				while (tA != nullptr)
				{
					cout << ' ' << tA->name << "=\"" << tA->value << '"';
					tA = tA->next;
				}
			}
			cout << '>' << endl;
			if (node->child != nullptr)
			{
				Print(node->child, count + 1, printAttribute, true);
			}
			for (size_t i = 0; i < count; ++i)
			{
				cout << "·";
			}
			cout << "</" << node->tagName << '>' << endl;
			if (printBrother && node->brother != nullptr)
			{
				Print(node->brother, count, printAttribute, printBrother);
			}
		}
	}
}