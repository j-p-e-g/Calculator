#include "stdafx.h"
#include "CalculatorView.h"
#include "MathNodes.h"

void CalculatorView::displayCalculation(const Node &i_rootNode) const
{
	std::cout << i_rootNode.getDescription() << std::endl;
}

void CalculatorView::outputResult(double i_result) const
{
	std::cout << "Result: " << i_result << std::endl;
}
