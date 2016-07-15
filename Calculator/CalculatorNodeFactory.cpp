#include "stdafx.h"
#include "CalculatorNodeFactory.h"

#include "Calculator.h"
#include "MathNodes.h"

CalculatorNodeFactory::CalculatorNodeFactory(Calculator &i_calculator) :
	m_calculator(&i_calculator)
{
	// bind some values
	m_calculator->addCommand('+', std::bind(&CalculatorNodeFactory::createAdditionNode, this));
	m_calculator->addCommand('-', std::bind(&CalculatorNodeFactory::createSubtractionNode, this));
	m_calculator->addCommand('*', std::bind(&CalculatorNodeFactory::createMultiplicationNode, this));
	m_calculator->addCommand('/', std::bind(&CalculatorNodeFactory::createDivisionNode, this));
}

void CalculatorNodeFactory::createAdditionNode() const
{
	createAndLinkNodeOfType(NodePriority::PRIO_ADDITION);
}

void CalculatorNodeFactory::createSubtractionNode() const
{
	createAndLinkNodeOfType(NodePriority::PRIO_SUBTRACTION);
}

void CalculatorNodeFactory::createMultiplicationNode() const
{
	createAndLinkNodeOfType(NodePriority::PRIO_MULTIPLICATION);
}

void CalculatorNodeFactory::createDivisionNode() const
{
	createAndLinkNodeOfType(NodePriority::PRIO_DIVISION);
}

void CalculatorNodeFactory::createAndLinkNodeOfType(NodePriority i_nodeType) const
{
	Node *parent = NULL, *leftChild = NULL;
	prepareNodeCreation(i_nodeType, &parent, &leftChild);
	Node* newNode = createAndReturnNodeOfType(i_nodeType, parent, leftChild, (Node*) new NodeValue(0));
	m_calculator->setMostRecentNode(newNode);
}

void CalculatorNodeFactory::prepareNodeCreation(NodePriority i_priority, Node **o_parentNode, Node **o_leftChild) const
{
	removeRecentEmptyOperationNode();
	Node* prevPrioNode = m_calculator->getEqualOrLowerPrioNode(i_priority);
	if (prevPrioNode->getNodePriority() > i_priority)
	{
		*o_parentNode = prevPrioNode->getParentNode();
		*o_leftChild  = prevPrioNode;
	}
	else
	{
		*o_parentNode = prevPrioNode;
		*o_leftChild  = prevPrioNode->getChildNode();
	}
}

// If the lastNode was an operation node and the second value is still zero,
// replace the operation node with a new one
void CalculatorNodeFactory::removeRecentEmptyOperationNode() const
{
	if (m_calculator->getMostRecentNode()->isEmpty())
	{
		m_calculator->removeRecentNode();
	}
}

Node* CalculatorNodeFactory::createAndReturnNodeOfType(NodePriority i_nodeType, Node *i_parent, Node *i_leftChild, Node *i_rightChild) const
{
	Node* newNode = NULL;
	switch (i_nodeType)
	{
	case PRIO_ADDITION:
		return (Node*) new NodeAddition(i_parent, i_leftChild, i_rightChild);
	case PRIO_SUBTRACTION:
		return (Node*) new NodeSubtraction(i_parent, i_leftChild, i_rightChild);
	case PRIO_MULTIPLICATION:
		return (Node*) new NodeMultiplication(i_parent, i_leftChild, i_rightChild);
	case PRIO_DIVISION:
		return (Node*) new NodeDivision(i_parent, i_leftChild, i_rightChild);
	default:
		_ASSERT(false, L"Unexpected node type*");
	}
}
