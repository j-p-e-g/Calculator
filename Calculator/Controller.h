#pragma once
#include "stdafx.h"

class Calculator;
class CalculatorView;
class Node;
class Controller
{
public:
	Controller(Calculator &i_calculator, CalculatorView &i_view) :
		m_calculator(&i_calculator),
		m_calculatorView(&i_view) {};

	char Controller::getInput() const;
	bool handleInput(const char i_input);

private:
	Calculator* m_calculator;
	CalculatorView* m_calculatorView;
};
