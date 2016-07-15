#pragma once
#include "stdafx.h"
#include <string>

enum NodePriority
{
	PRIO_ADDITION = 1,
	PRIO_SUBTRACTION,
	PRIO_MULTIPLICATION,
	PRIO_DIVISION,
	PRIO_NONE
};

class Node
{
public:
	// C'tor
	Node() : m_parentNode(NULL) {}
	Node(Node* i_parentNode);
	virtual ~Node() {}

	// abstract methods
	virtual void addDigit(int i_digit) = 0;
	virtual void addComma() = 0;
	virtual double calculate() const = 0;
	virtual const std::string getDescription() const = 0;
	virtual void setChildNode(Node &i_childNode) = 0;
	virtual Node* getChildNode() const = 0;

	// returns the node that will replace the current node if the current one is unlinked
	// (only may happen if the current node is the rightmost non-empty node in the tree)
	virtual Node* getUnlinkReplacementNode() const = 0;
	virtual void unlinkNode() = 0;
	virtual bool handleBackspace() = 0;
	virtual Node* getLatestValueNode() const = 0;
	virtual Node* getRightmostOpNode() const = 0;
	virtual bool isEmpty() const = 0;
	virtual NodePriority getNodePriority() const = 0;

	// inlined methods
	inline Node* getParentNode() const { return m_parentNode; }
	inline void setParentNode(Node* i_parentNode) { m_parentNode = i_parentNode; }

	void relinkNode(Node* i_newParentNode);
	Node* getRootNode() const;

private:
	Node* m_parentNode;
};

class NodeValue : protected Node
{
public:
	// C'tor
	NodeValue(int i_preCommaValue);
	NodeValue(int i_preCommaValue, Node* i_parentNode);

	// inlined methods
	// this should never happen
	inline void setChildNode(Node &i_childNode) { throw __ExceptionPtrCurrentException; }

	inline Node* getChildNode() const { return NULL; }
	inline Node* getRightmostOpNode() const { return NULL; }
	inline Node* getLatestValueNode() const { return const_cast<Node*>((Node*)this); }

	inline NodePriority getNodePriority() const { return NodePriority::PRIO_NONE; };
	inline bool isEmpty() const { return m_preCommaValue == 0; }

	void addDigit(int i_digit);
	void addComma();
	double calculate() const;
	const std::string getDescription() const;
	Node* getUnlinkReplacementNode() const;
	void unlinkNode();
	bool handleBackspace();

private:
	void initValues(int i_preCommaValue, int i_postCommaValue, int i_postCommaDigits);

	// returns whether a digit was removed
	bool removeDigit();

	int m_preCommaValue;
	int m_postCommaValue;
	int m_postCommaDigits;
};

class NodeOp : protected Node
{
public:
	~NodeOp();

	// inline methods
	inline void setChildNode(Node &i_childNode) { m_secondNode = &i_childNode; }
	inline Node* getChildNode() const { return m_secondNode; }
	inline Node* getUnlinkReplacementNode() const { return m_firstNode; }
	inline NodePriority getNodePriority() const { return m_priority; };

	Node* getLatestValueNode() const;
	Node* getRightmostOpNode() const;
	void addDigit(int i_digit);
	void addComma();
	bool handleBackspace();
	void unlinkNode();
	bool isEmpty() const;
	const std::string getDescription() const;

protected:
	// C'tor
	NodeOp(Node *i_firstNode, Node *i_secondNode, NodePriority i_priority, const char i_glyph);
	NodeOp(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode, NodePriority i_priority, const char i_glyph);

	inline virtual const char getGlyph() const { return m_glyph; }

	double calculateFirst() const;
	double calculateSecond() const;

	Node* m_firstNode;
	Node* m_secondNode;

private:
	NodePriority m_priority;
	const char m_glyph;
};

class NodeAddition : public NodeOp
{
public:
	NodeAddition(Node *i_firstNode, Node *i_secondNode) : NodeAddition(NULL, i_firstNode, i_secondNode) {}

	NodeAddition(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode) : NodeOp(i_parentNode, i_firstNode, i_secondNode, NodePriority::PRIO_ADDITION, '+') {}

	double calculate() const;
};

class NodeSubtraction : protected NodeOp
{
public:
	NodeSubtraction(Node *i_firstNode, Node *i_secondNode) : NodeSubtraction(NULL, i_firstNode, i_secondNode) {}

	NodeSubtraction(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode) : NodeOp(i_parentNode, i_firstNode, i_secondNode, NodePriority::PRIO_SUBTRACTION, '-') {}

	double calculate() const;
};

class NodeMultiplication : protected NodeOp
{
public:
	NodeMultiplication(Node *i_firstNode, Node *i_secondNode) : NodeMultiplication(NULL, i_firstNode, i_secondNode) {}

	NodeMultiplication(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode) : NodeOp(i_parentNode, i_firstNode, i_secondNode, NodePriority::PRIO_MULTIPLICATION, '*') {}

	double calculate() const;
};

class NodeDivision : protected NodeOp
{
public:
	NodeDivision(Node *i_firstNode, Node *i_secondNode) : NodeDivision(NULL, i_firstNode, i_secondNode) {}

	NodeDivision(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode) : NodeOp(i_parentNode, i_firstNode, i_secondNode, NodePriority::PRIO_DIVISION, '/') {}

	double calculate() const;
};
