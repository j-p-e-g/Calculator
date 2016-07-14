#include "stdafx.h"
#include "MathNodes.h"

Node::Node(Node* i_parentNode) : m_parentNode(i_parentNode)
{
	if (m_parentNode)
	{
		m_parentNode->setChildNode(*this);
	}
}

void Node::relinkNode(Node* i_newParentNode)
{
	setParentNode(i_newParentNode);
	if (i_newParentNode)
	{
		i_newParentNode->setChildNode(*this);
	}
}

Node* Node::getRootNode() const
{
	if (m_parentNode)
	{
		return m_parentNode->getRootNode();
	}
	return const_cast<Node*>(this);
}


NodeValue::NodeValue(int i_preCommaValue) : Node()
{
	initValues(i_preCommaValue, 0, 0);
}

NodeValue::NodeValue(int i_preCommaValue, Node* i_parentNode) : Node(i_parentNode)
{
	initValues(i_preCommaValue, 0, 0);
}

void NodeValue::initValues(int i_preCommaValue, int i_postCommaValue, int i_postCommaDigits)
{
	m_preCommaValue = i_preCommaValue;
	m_postCommaValue = i_postCommaValue;
	m_postCommaDigits = i_postCommaDigits;
}

void NodeValue::addDigit(int i_digit)
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

void NodeValue::addComma()
{
	if (m_postCommaDigits == 0)
	{
		m_postCommaDigits = 1;
	}
}

double NodeValue::calculate() const
{
	// cast to double
	if (m_postCommaDigits < 2)
	{
		return m_preCommaValue;
	}
	double postCommaValue = m_postCommaValue / pow(10, m_postCommaDigits - 1);
	return (double)m_preCommaValue + postCommaValue;
}

const std::string NodeValue::getDescription() const
{
	char desc[20];
	if (m_postCommaDigits == 0)
	{
		snprintf(desc, sizeof(desc), "%d", (int)m_preCommaValue);
	}
	else
	{
		snprintf(desc, sizeof(desc), "%.2f", calculate());
	}
	return desc;
}

Node* NodeValue::getUnlinkReplacementNode() const
{
	if (m_preCommaValue == 0 && getParentNode())
	{
		return getParentNode()->getUnlinkReplacementNode();
	}
	return getParentNode();
}

void NodeValue::unlinkNode()
{
	if (getParentNode())
	{
		getParentNode()->unlinkNode();
	}
}

bool NodeValue::handleBackspace()
{
	return !removeDigit();
}

bool NodeValue::removeDigit()
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


NodeOp::NodeOp(Node *i_firstNode, Node *i_secondNode, NodePriority i_priority, const char i_glyph) : Node(),
	m_firstNode(i_firstNode),
	m_secondNode(i_secondNode),
	m_priority(i_priority),
	m_glyph(i_glyph)
{
	m_firstNode->setParentNode(this);
	m_secondNode->setParentNode(this);
}

NodeOp::NodeOp(Node* i_parentNode, Node *i_firstNode, Node *i_secondNode, NodePriority i_priority, const char i_glyph) : Node(i_parentNode),
	m_firstNode(i_firstNode),
	m_secondNode(i_secondNode),
	m_priority(i_priority),
	m_glyph(i_glyph)
{
	m_firstNode->setParentNode(this);
	m_secondNode->setParentNode(this);
}

NodeOp::~NodeOp()
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

Node* NodeOp::getRightmostOpNode() const
{
	Node* rightmostChild = m_secondNode->getRightmostOpNode();
	return (rightmostChild ? rightmostChild : (Node*) this);
}

void NodeOp::addDigit(int i_digit)
{
	m_secondNode->addDigit(i_digit);
}

void NodeOp::addComma()
{
	m_secondNode->addComma();
}

Node* NodeOp::getLatestValueNode() const
{
	return m_secondNode->getLatestValueNode();
}

bool NodeOp::handleBackspace()
{
	return m_secondNode->handleBackspace();
}

void NodeOp::unlinkNode()
{
	m_firstNode->relinkNode(getParentNode());
	m_firstNode = NULL;
	m_secondNode = NULL;
}

bool NodeOp::isEmpty() const
{
	return m_secondNode->isEmpty();
}

const std::string NodeOp::getDescription() const
{
	char desc[200];
	snprintf(desc, sizeof(desc), "%s %c %s", (m_firstNode ? m_firstNode->getDescription().c_str() : ""), getGlyph(), (m_secondNode ? m_secondNode->getDescription().c_str() : ""));
	return desc;
}

double NodeOp::calculateFirst() const
{
	return m_firstNode ? m_firstNode->calculate() : 0;
}

double NodeOp::calculateSecond() const
{
	return m_secondNode ? m_secondNode->calculate() : calculateFirst();
}

double NodeAddition::calculate() const
{
	return calculateFirst() + calculateSecond();
}

double NodeSubtraction::calculate() const
{
	return calculateFirst() - calculateSecond();
}

double NodeMultiplication::calculate() const
{
	return calculateFirst() * calculateSecond();
}

double NodeDivision::calculate() const
{
	return calculateFirst() / calculateSecond();
}
