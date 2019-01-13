#include "MLParser.h"
#include "Scanner.hpp"
#include <stack>
#include <regex>
#include <list>
using std::list;
using std::stack;
using std::string;
namespace Cyan
{
	void MLParser::preprocess(string &m)
	{
		//���´�������ȥ��script��ǩ��style��ǩ��ע��
		std::regex exp1("<script([\\S\\s]+?)<\\/script>");
		m = std::regex_replace(m, exp1, "");
		std::regex exp2("<!--([\\S\\s]+?)-->");
		m = std::regex_replace(m, exp2, "");
		std::regex exp3("<!([\\S\\s]+?)>");
		m = std::regex_replace(m, exp3, "");
		std::regex exp4("<style([\\S\\s]+?)<\\/style>");
		m = std::regex_replace(m, exp4, "");

		raw = Cyan::strcpy(m.data());
		//���´�������ȥ��\n \r \t
		size_t i, k;
		for (i = k = 0; raw[i] != '\0'; ++i)
		{
			if (raw[i] == '\n' || raw[i] == '\r' || raw[i] == '\t') {}
			else
			{
				raw[k] = raw[i];
				k = k + 1;
			}
		}
		raw[k] = '\0';
	}
	bool MLParser::Parse(string html)
	{
		preprocess(html);
		//initial Scanner & Scan
		Scanner SC = Scanner(raw);
		SC.Scan();

		stack<Node *> NodeStack;
		list<Node *> NodeList;
		root = new Node();
		root->tagName = "_root_";
		Node *lNode = root;//��¼��һ����������Node
		Node *pNode = root;//��һ���½ڵ�ĸ��׽ڵ�
		Node *tNode = root;//���ڸ���Node��Ϣ
		NodeStack.push(root);
		Attribute *lAttribute = nullptr;//��¼��һ����������Attribute;

		//****************************************
		//��Ϊ��ƴ��󣬲��ò�ʹ�øñ�����¼һЩ��Ϣ
		//����LeftAngleBracket��EndTag��ı����ֵ����ΪL_A_B����EndTag��
		//����RightAngleBracket��ʹ�����ֵ��
		//����ñ���(tType) == LeftAngleBracket
		//��ôRightAngleBracket���޸�lNode��txtOffset�ֶ�Ϊ�������offset
		//��֮�����κβ���
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
				tType = LeftAngleBracket;//��tType����ǰע��
				break;
			case AttributeName:
				lAttribute = lNode->AddAttribute(token->value, "");
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
					if (it == NodeList.end())
					{
						//���˵tNode->tagName != token->value���Ͳ������it == NodeList.end()
						//˵������ƾ�ճ��ֵ�һ��End_Tag
						//���ƾ�ճ��ֵ�End_Tag��Ӧ������κ�Ӱ��
						//�������ǰѵ�������Node�ټӻ�ȥ
						NodeStack.push(tNode);
						NodeList.push_back(tNode);
						break;
					}
					//����Ĵ��������޸�parent��ָ�����
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
				tType = EndTag;//��tType����ǰע��
				break;
			case RightAngleBracket:
				if (tType == LeftAngleBracket)//��tType����������ע��
				{
					lNode->txtOffset = token->offset + 1;//token->offsetָ���Ҽ����ţ�+1��Ϊָ�����ݵĵ�һ���ַ�
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
				cout << "��";
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
				cout << "��";
			}
			cout << "</" << node->tagName << '>' << endl;
			if (printBrother && node->brother != nullptr)
			{
				Print(node->brother, count, printAttribute, printBrother);
			}
		}
	}
}