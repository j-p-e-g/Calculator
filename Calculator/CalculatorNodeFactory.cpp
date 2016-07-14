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

// If the lastNode was an operation node and the second value is still zero,
// replace the operation node with a new one
void CalculatorNodeFactory::removeRecentEmptyOperationNode() const
{
	if (m_calculator->getMostRecentNode()->isEmpty())
	{
		m_calculator->removeRecentNode();
	}
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

void CalculatorNodeFactory::createAdditionNode() const
{
	Node *parent = m_calculator->getMostRecentNode(), *leftChild = parent;
	prepareNodeCreation(NodePriority::PRIO_ADDITION, &parent, &leftChild);
	Node* newNode = (Node*) new NodeAddition(parent, leftChild, (Node*) new NodeValue(0));
	m_calculator->setMostRecentNode(newNode);
}

void CalculatorNodeFactory::createSubtractionNode() const
{
	Node *parent = NULL, *leftChild = NULL;
	prepareNodeCreation(NodePriority::PRIO_ADDITION, &parent, &leftChild);
	Node* newNode = (Node*) new NodeSubtraction(parent, leftChild, (Node*) new NodeValue(0));
	m_calculator->setMostRecentNode(newNode);
}

void CalculatorNodeFactory::createMultiplicationNode() const
{
	Node *parent = NULL, *leftChild = NULL;
	prepareNodeCreation(NodePriority::PRIO_MULTIPLICATION, &parent, &leftChild);
	Node* newNode = (Node*) new NodeMultiplication(parent, leftChild, (Node*) new NodeValue(0));
	m_calculator->setMostRecentNode(newNode);
}

void CalculatorNodeFactory::createDivisionNode() const
{
	Node *parent = NULL, *leftChild = NULL;
	prepareNodeCreation(NodePriority::PRIO_DIVISION, &parent, &leftChild);
	Node* newNode = (Node*) new NodeDivision(parent, leftChild, (Node*) new NodeValue(0));
	m_calculator->setMostRecentNode(newNode);
}

