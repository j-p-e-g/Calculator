#pragma once
#include "stdafx.h"
#include "MathNodes.h"
#include <map>
#include <functional>

typedef std::function<void(void)> FunctionRef;

class Calculator
{
public:
	Calculator();

	void addCommand(const char i_input, FunctionRef i_binding);
	Node* getMostRecentNode() const { return m_mostRecentNode; }
	void setMostRecentNode(Node* i_node) { m_mostRecentNode = i_node; }
	Node* getRootNode() const { return m_mostRecentNode ? m_mostRecentNode->getRootNode() : NULL; }
	Node* getRightmostNode() const;

	void handleEnteredDigit(int i_digit);
	bool handleNonDigit(const char i_key);
	void addComma();
	double calculateResult() const;
	void clearCalculation();
	void handleBackspace();
	void removeRecentNode();
	Node* getEqualOrLowerPrioNode(const NodePriority i_prio) const;

private:
	Node* m_mostRecentNode;

	std::map< const char, FunctionRef > m_nodeMap;
};

