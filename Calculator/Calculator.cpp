// Calculator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Calculator.h"
#include "CalculatorNodeFactory.h"
#include "CalculatorView.h"
#include "Controller.h"
#include <iostream>

int main()
{
	// TODO: pass CalculatorView into Calculator (instead of Controller)
	// handle output directly in response to things changing in the calculator
	CalculatorView* calculatorView = new CalculatorView();
	Calculator* calculator         = new Calculator();
	Controller* controller         = new Controller(*calculator, *calculatorView);
	CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

	calculatorView->displayCalculation(*calculator->getRootNode());

	// aborts if the user wants to quit
	char input;
	do
	{
		input = controller->getInput();
	}
	while (controller->handleInput(input));

	system("pause");
    return 0;
}

Calculator::Calculator()
{
	// initialize the Calculator with a valid value node
	// this violates the rule of not doing work inside the constructor
	// but the zero should be harmless and basically functions as a NULL object
	m_mostRecentNode = (Node*) new NodeValue(0);
	addCommand('\b', std::bind(&Calculator::handleBackspace, this));
	addCommand('.', std::bind(&Calculator::addComma, this));
	addCommand(',', std::bind(&Calculator::addComma, this));
}

void Calculator::addCommand(const char i_input, FunctionRef i_binding)
{
	m_nodeMap[i_input] = i_binding;
}

void Calculator::handleEnteredDigit(int i_digit)
{
	m_mostRecentNode->addDigit(i_digit);
}

void Calculator::addComma()
{
	m_mostRecentNode->addComma();
}

bool Calculator::handleNonDigit(const char i_key)
{
	if (m_nodeMap.find(i_key) != m_nodeMap.end())
	{
		m_nodeMap[i_key]();
		return true;
	}
	return false;
}

double Calculator::calculateResult() const
{
	return m_mostRecentNode->getRootNode()->calculate();
}

void Calculator::clearCalculation()
{
	delete m_mostRecentNode->getRootNode();
	m_mostRecentNode = (Node*) new NodeValue(0);
}

Node* Calculator::getRightmostNode() const
{
	Node* current = getRootNode();
	while (current->getChildNode())
	{
		current = current->getChildNode();
	}
	return current;
}

void Calculator::handleBackspace()
{
	// handleBackspace returns true only if the node needs to be removed
	if (m_mostRecentNode->handleBackspace() && m_mostRecentNode->getUnlinkReplacementNode()) 
	{
		removeRecentNode();
	}
}

void Calculator::removeRecentNode()
{
	// FIXME: this process is very complex and likely to fall apart on touch
	// keep a copy before unlinking the node
	Node* replacementNode = m_mostRecentNode->getUnlinkReplacementNode();

	m_mostRecentNode->unlinkNode();
	delete m_mostRecentNode;
	// update the most recent node
	Node* rightmostNode = replacementNode->getRightmostOpNode();
	m_mostRecentNode = (rightmostNode ? rightmostNode : replacementNode);
}

Node* Calculator::getEqualOrLowerPrioNode(const NodePriority i_prio) const
{
	Node* rightmostValueNode = m_mostRecentNode->getLatestValueNode();

	// returns the last node that has a priority <= the given priority
	Node* curr = rightmostValueNode;
	while (curr->getNodePriority() > i_prio && curr->getParentNode())
	{
		curr = curr->getParentNode();
	}
	return curr;
}
