#include "stdafx.h"
#include "CppUnitTest.h"
#include "../Calculator/Calculator.cpp"
#include "../Calculator/CalculatorNodeFactory.cpp"
#include "../Calculator/CalculatorView.cpp"
#include "../Calculator/Controller.cpp"
#include "../Calculator/MathNodes.h"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CalculatorTest
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(IntegrationTest_Controller_Calculator)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			const std::vector<char> fixedInput = { '3', '+', '2', '*', '4', '-', '2', '/', '3' };
			const std::string desc = "3 + 2 * 4 - 2 / 3";
			for (std::vector<char>::const_iterator it = fixedInput.begin(), end = fixedInput.end(); it != end; it++)
			{
				controller->handleInput(*it);
			}
			Assert::AreEqual(desc, calculator->getRootNode()->getDescription(), L"Unexpected description for calculation");
			Assert::AreEqual(10.3333, calculator->calculateResult(), 0.001, L"Wrong result in calculator test");
		}

		TEST_METHOD(IntegrationTest_Backspace_Test)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			Node* a = (Node*) new NodeValue(1);
			Node* b = (Node*) new NodeValue(2);
			Node* c = (Node*) new NodeValue(3);
			Node* d = (Node*) new NodeValue(4);
			Node* e = (Node*) new NodeValue(5);
			Node* multiplyNode = (Node*) new NodeMultiplication(b, c);
			Node* divideNode = (Node*) new NodeDivision(d, e);
			Node* addNode = (Node*) new NodeAddition(a, multiplyNode);
			Node* subtractNode = (Node*) new NodeSubtraction(addNode, divideNode);
			calculator->setMostRecentNode(divideNode);

			const std::vector<std::string> backspaceResults = {
				"1 + 2 * 3 - 4 / 0",
				"1 + 2 * 3 - 4",
				"1 + 2 * 3 - 0",
				"1 + 2 * 3",
				"1 + 2 * 0",
				"1 + 2",
				"1 + 0",
				"1",
				"0"
			};
			for (std::vector<std::string>::const_iterator it = backspaceResults.begin(), end = backspaceResults.end(); it != end; it++)
			{
				controller->handleInput('\b');
				Assert::AreEqual(*it, calculator->getRootNode()->getDescription(), L"Unexpected result after executing backspace");
			}
		}

		TEST_METHOD(IntegrationTest_ReplaceOperationNode)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			Node* a = (Node*) new NodeValue(1);
			Node* b = (Node*) new NodeValue(2);
			Node* multiplyNode = (Node*) new NodeMultiplication(a, b);
			calculator->setMostRecentNode(multiplyNode);

			controller->handleInput('+');
			Assert::AreEqual("1 * 2 + 0", calculator->getRootNode()->getDescription().c_str(), L"Unexpected description when adding operation symbol");

			controller->handleInput('/');
			Assert::AreEqual("1 * 2 / 0", calculator->getRootNode()->getDescription().c_str(), L"Unexpected description when replacing operation symbol");
		}

		TEST_METHOD(ValueNode_addDigit_toInt_Test)
		{
			Node* valueNode = (Node*) new NodeValue(3);
			valueNode->addDigit(0);
			valueNode->addDigit(2);

			Assert::AreEqual(302.0, valueNode->calculate(), L"Adding digits badly handled");
		}

		TEST_METHOD(ValueNode_addDigit_toDouble_Test)
		{
			Node* valueNode = (Node*) new NodeValue(3);
			valueNode->addDigit(0);
			valueNode->addComma();
			valueNode->addDigit(2);

			Assert::AreEqual(30.2, valueNode->calculate(), L"Adding digits badly handled");
		}

		TEST_METHOD(IntegrationTest_Priority_AddMultiply)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			// 2 + 3*4 = 14
			const std::vector<char> fixedInput = { '2', '+', '3', '*', '4' };
			for (std::vector<char>::const_iterator it = fixedInput.begin(), end = fixedInput.end(); it != end; it++)
			{
				controller->handleInput(*it);
			}
			Assert::AreEqual("2 + 3 * 4", calculator->getRootNode()->getDescription().c_str(), L"Unexpected description in calculator test");
			Assert::AreEqual(14, calculator->calculateResult(), 0.001, L"Wrong result in calculator test");
		}

		TEST_METHOD(IntegrationTest_Priority_MultiplyAdd)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			// 2*3 + 4 = 10
			const std::vector<char> fixedInput = { '2', '*', '3', '+', '4' };
			for (std::vector<char>::const_iterator it = fixedInput.begin(), end = fixedInput.end(); it != end; it++)
			{
				controller->handleInput(*it);
			}
			Assert::AreEqual("2 * 3 + 4", calculator->getRootNode()->getDescription().c_str(), L"Unexpected description in calculator test");
			Assert::AreEqual(10, calculator->calculateResult(), 0.001, L"Wrong result in calculator test");
		}

		TEST_METHOD(IntegrationTest_Priority_MultiplyDivide)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			// 8 * 4/2 = 16
			const std::vector<char> fixedInput = { '8', '*', '4', '/', '2' };
			for (std::vector<char>::const_iterator it = fixedInput.begin(), end = fixedInput.end(); it != end; it++)
			{
				controller->handleInput(*it);
			}
			Assert::AreEqual("8 * 4 / 2", calculator->getRootNode()->getDescription().c_str(), L"Unexpected description in calculator test");
			Assert::AreEqual(16, calculator->calculateResult(), 0.001, L"Wrong result in calculator test");
		}

		TEST_METHOD(IntegrationTest_Priority_DivideMultiply)
		{
			CalculatorView* calculatorView = (CalculatorView*) new MockCalculatorView();
			Calculator* calculator = new Calculator();
			Controller* controller = new Controller(*calculator, *calculatorView);
			CalculatorNodeFactory* factory = new CalculatorNodeFactory(*calculator);

			// 8/4 * 2 = 4
			const std::vector<char> fixedInput = { '8', '/', '4', '*', '2' };
			for (std::vector<char>::const_iterator it = fixedInput.begin(), end = fixedInput.end(); it != end; it++)
			{
				controller->handleInput(*it);
			}
			Assert::AreEqual("8 / 4 * 2", calculator->getRootNode()->getDescription().c_str(), L"Unexpected description in calculator test");
			Assert::AreEqual(4, calculator->calculateResult(), 0.001, L"Wrong result in calculator test");
		}
	};
}