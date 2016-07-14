#pragma once
#include "stdafx.h"
#include <string>

enum NodePriority
{
	PRIO_ADDITION = 1, // also subtraction
	PRIO_MULTIPLICATION,
	PRIO_DIVISION,
	PRIO_NONE
};

class Node
{
public:
	Node() : m_parentNode(NULL) {}
	Node(Node* i_parentNode) : m_parentNode(i_parentNode)
	{
		if (m_parentNode)
		{
			m_parentNode->setChildNode(*this);
		}
	}

	virtual ~Node() {}

	virtual void initNode() {}

	virtual void addDigit(int i_digit) = 0;
	virtual void addComma() = 0;
	virtual double calculate() const = 0;
	virtual const std::string getDescription() const = 0;
	Node* getParentNode() const { return m_parentNode; }
	void setParentNode(Node* i_parentNode) { m_parentNode = i_parentNode; }
	virtual void setChildNode(Node &i_childNode) = 0;
	virtual Node* getChildNode() const = 0;
	// the node that will replace the current node if the current one is unlinked
	// (only may happen if the current node is the rightmost non-empty node in the tree)
	virtual Node* getUnlinkReplacementNode() const = 0;

	virtual void relinkNode(Node* i_newParentNode)
	{
		setParentNode(i_newParentNode);
		if (i_newParentNode)
		{
			i_newParentNode->setChildNode(*this);
		}
	}

	virtual void unlinkNode() = 0;
	virtual bool handleBackspace() = 0;

	Node* getRootNode() const
	{
		if (m_parentNode)
		{
			return m_parentNode->getRootNode();
		}
		return const_cast<Node*>(this);
	}
	virtual Node* getLatestValueNode() const = 0;
	virtual Node* getRightmostOpNode() const = 0;
	virtual bool isEmpty() const = 0;
	virtual NodePriority getNodePriority() const = 0;

	//Node* getEqualOrLowerPrioParentNode(const NodePriority i_prio) const
	//{
	//	if (getParentNode() == NULL || getNodePriority() <= i_prio)
	//	{
	//		return (Node*)this;
	//	}
	//	return getParentNode()->getEqualOrLowerPrioParentNode(i_prio);
	//}

private:
	Node* m_parentNode;
};

class NodeValue : protected Node
{
public:
	NodeValue(int i_preCommaValue) : Node()
	{
		initValues(i_preCommaValue, 0, 0);
	}

	NodeValue(int i_preCommaValue, Node* i_parentNode) : Node(i_parentNode)
	{
		initValues(i_preCommaValue, 0, 0);
	}

	// this should never happen
	void setChildNode(Node &i_childNode) { throw __ExceptionPtrCurrentException; }

	Node* getChildNode() const { return NULL; }

	Node* getLatestValueNode() const
	{
		return const_cast<Node*>((Node*)this);
	}

	Node* getRightmostOpNode() const
	{
		return NULL;
	}

	void addDigit(int i_digit)
	{
		if (m_postCommaDigits > 0)
		{
			m_postCommaValue = m_postCommaValue * 10 + i_digit;
			m_postCommaDigits++;
		}
		else
		{
			m_preCommaValue = m_preCommaValue * 10 + i_digit;
		}
	}

	virtual void addComma()
	{
		if (m_postCommaDigits == 0)
		{
			m_postCommaDigits = 1;
		}
	}

	double calculate() const
	{
		// cast to double
		if (m_postCommaDigits < 2)
		{
			return m_preCommaValue;
		}
		double postCommaValue = m_postCommaValue / pow(10, m_postCommaDigits-1);
		return (double) m_preCommaValue + postCommaValue;
	}

	const std::string getDescription() const
	{
		char desc[20];
		if (m_postCommaDigits == 0)
		{
			snprintf(desc, sizeof(desc), "%d", (int) m_preCommaValue);
		}
		else
		{
			snprintf(desc, sizeof(desc), "%.2f", calculate());
		}
		return desc;
	}

	NodePriority getNodePriority() const
	{
		return NodePriority::PRIO_NONE;
	};

	bool isEmpty() const { return m_preCommaValue == 0; }

	Node* getUnlinkReplacementNode() const
	{
		if (m_preCommaValue == 0 && getParentNode())
		{
			return getParentNode()->getUnlinkReplacementNode();
		}
		return getParentNode();
	}

	void unlinkNode()
	{
		if (getParentNode())
		{
			getParentNode()->unlinkNode();
			//getParentNode()->setChildNode(*(Node*) new NodeValue(0));
		}
	}

	bool handleBackspace()
	{
		return !removeDigit();
	}

private:
	void initValues(int i_preCommaValue, int i_postCommaValue, int i_postCommaDigits)
	{
		m_preCommaValue = i_preCommaValue;
		m_postCommaValue = i_postCommaValue;
		m_postCommaDigits = i_postCommaDigits;
	}

	// returns whether a digit was removed
	bool removeDigit()
	{
		if (m_postCommaDigits > 0)
		{
			m_postCommaValue = (int)m_postCommaValue / 10;
			m_postCommaDigits--;
			return true;
		}
		else if (m_preCommaValue > 0)
		{
			m_preCommaValue = (int)m_preCommaValue / 10;
			return true;
		}
		return false;
	}

	int m_preCommaValue;
	int m_postCommaValue;
	int m_postCommaDigits;
};

class NodeOp : protected Node
{
public:
	//explicit NodeOp() : Node() {}

	explicit NodeOp(Node *i_firstNode, Node *i_secondNode) : Node(),
		m_firstNode(i_firstNode),
		m_secondNode(i_secondNode),
		m_priority(NodePriority::PRIO_NONE)
	{
		m_firstNode->setParentNode(this);
		m_secondNode->setParentNode(this);
	}

	explicit NodeOp(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode) : Node(i_parentNode),
		m_firstNode(i_firstNode),
		m_secondNode(i_secondNode),
		m_priority(NodePriority::PRIO_NONE)
	{
		m_firstNode->setParentNode(this);
		m_secondNode->setParentNode(this);
	}

	virtual void initNode() {};

	~NodeOp()
	{
		if (m_firstNode)
		{
			delete m_firstNode;
			m_firstNode = NULL;
		}
		if (m_secondNode)
		{
			delete m_secondNode;
			m_secondNode = NULL;
		}
	}

	void setChildNode(Node &i_childNode)
	{
		m_secondNode = &i_childNode;
	}

	Node* getChildNode() const
	{
		return m_secondNode;
	}

	Node* getLatestValueNode() const
	{
		return m_secondNode->getLatestValueNode();
	}

	Node* getRightmostOpNode() const
	{
		Node* rightmostChild = m_secondNode->getRightmostOpNode();
		return (rightmostChild ? rightmostChild : (Node*) this);
	}

	void addDigit(int i_digit)
	{
		m_secondNode->addDigit(i_digit);
	}

	void addComma()
	{
		m_secondNode->addComma();
	}

	void setSecondNode(Node &i_Node)
	{
		m_secondNode = &i_Node;
	}

	bool handleBackspace()
	{
		return m_secondNode->handleBackspace();
	}

	Node* getUnlinkReplacementNode() const
	{
		return m_firstNode;
	}

	void relinkNode(Node* i_newParentNode)
	{
		Node::relinkNode(i_newParentNode);
		//setChildNode(*(Node*) new NodeValue(0));
	}

	void unlinkNode()
	{
		m_firstNode->relinkNode(getParentNode());
		m_firstNode = NULL;
		m_secondNode = NULL;
	}

	NodePriority getNodePriority() const
	{
		return m_priority;
	};

	bool isEmpty() const { return m_secondNode->isEmpty(); }

	virtual const char getGlyph() const { return ' '; }

	const std::string getDescription() const
	{
		char desc[200];
		snprintf(desc, sizeof(desc), "%s %c %s", (m_firstNode ? m_firstNode->getDescription().c_str() : ""), getGlyph(), (m_secondNode ? m_secondNode->getDescription().c_str() : ""));
		return desc;
	}

protected:
	double calculateFirst() const
	{
		return m_firstNode ? m_firstNode->calculate() : 0;
	}
	double calculateSecond() const
	{
		return m_secondNode ? m_secondNode->calculate() : calculateFirst();
	}

	Node* m_firstNode;
	Node* m_secondNode;
	NodePriority m_priority;
};

class NodeAddition : public NodeOp
{
public:
	using NodeOp::NodeOp;

	//NodeAddition(Node *i_firstNode, Node *i_secondNode) : NodeOp(i_firstNode, i_secondNode)
	//{
	//	m_priority = NodePriority::PRIO_ADDITION;
	//}

	//NodeAddition(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode) : NodeOp(i_parentNode, i_firstNode, i_secondNode)
	//{
	//	m_priority = NodePriority::PRIO_ADDITION;
	//}

	void initNode()
	{
		m_priority = NodePriority::PRIO_ADDITION;
	}

	double calculate() const
	{
		return calculateFirst() + calculateSecond();
	}

protected:
	const char getGlyph() const { return '+'; }
};

class NodeSubtraction : protected NodeOp
{
public:
	using NodeOp::NodeOp;

	void initNode()
	{
		m_priority = NodePriority::PRIO_ADDITION;
	}

	double calculate() const
	{
		return calculateFirst() - calculateSecond();
	}

protected:
	const char getGlyph() const { return '-'; }
};

class NodeMultiplication : protected NodeOp
{
public:
	using NodeOp::NodeOp;

	void initNode()
	{
		m_priority = NodePriority::PRIO_MULTIPLICATION;
	}

	double calculate() const
	{
		return calculateFirst() * calculateSecond();
	}

protected:
	const char getGlyph() const { return '*'; }
};

class NodeDivision : protected NodeOp
{
public:
	using NodeOp::NodeOp;

	void initNode()
	{
		m_priority = NodePriority::PRIO_DIVISION;
	}

	double calculate() const
	{
		return calculateFirst() / calculateSecond();
	}

protected:
	const char getGlyph() const { return '/'; }
};
