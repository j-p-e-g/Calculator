#include "stdafx.h"
#include "Controller.h"
#include "Calculator.h"
#include "CalculatorView.h"
#include "MathNodes.h"
#include <functional>
#include <ctype.h>
#include <conio.h>
#include <iostream>

char Controller::getInput() const
{
	return _getch();
}

bool Controller::handleInput(const char i_input)
{
	if (isdigit(i_input))
	{
		int number = i_input - '0';
		m_calculator->handleEnteredDigit(number);
		m_calculatorView->displayCalculation(*m_calculator->getRootNode());
	}
	else
	{
		switch (i_input)
		{
		case 'q':
		case 'Q':
			// quit
			return false;
		case '=':
			m_calculatorView->outputResult(m_calculator->calculateResult());
			m_calculator->clearCalculation();
			break;
		default:
			if (m_calculator->handleNonDigit(i_input))
			{
				m_calculatorView->displayCalculation(*m_calculator->getRootNode());
			}
			else
			{
				std::cout << "Invalid input: " << i_input << std::endl;
			}
		}
	}
	return true;
}