#pragma once
#include "stdafx.h"
#include <iostream>

class Node;
class CalculatorView
{
public:
	virtual void displayCalculation(const Node &i_rootNode) const;
	virtual void outputResult(double i_result) const;
};

// for testing
class MockCalculatorView : CalculatorView
{
public:
	void displayCalculation(const Node &i_rootNode) const {};
	void outputResult(double i_result) const {};
};