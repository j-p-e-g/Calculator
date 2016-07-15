#pragma once
#include "stdafx.h"

class Calculator;
class Node;
enum NodePriority;
class CalculatorNodeFactory
{
public:
	CalculatorNodeFactory(Calculator &i_calculator);

private:
	void createAdditionNode() const;
	void createSubtractionNode() const;
	void createMultiplicationNode() const;
	void createDivisionNode() const;

	void createAndLinkNodeOfType(NodePriority i_nodeType) const;
	void prepareNodeCreation(NodePriority i_priority, Node **o_parentNode, Node **o_leftChild) const;
	void removeRecentEmptyOperationNode() const;
	Node* createAndReturnNodeOfType(NodePriority i_nodeType, Node *i_parent, Node *i_leftChild, Node *i_rightChild) const;

	Calculator* m_calculator;
};