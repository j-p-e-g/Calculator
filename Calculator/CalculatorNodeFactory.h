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
	void prepareNodeCreation(NodePriority i_priority, Node **o_parentNode, Node **o_leftChild) const;
	void removeRecentEmptyOperationNode() const;
	void createAdditionNode() const;
	void createSubtractionNode() const;
	void createMultiplicationNode() const;
	void createDivisionNode() const;

	Calculator* m_calculator;
};