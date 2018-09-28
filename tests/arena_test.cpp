/* g++ tests/arena_test.cpp -o arena_test -std=c++11
*  ./arena_test
*/

#include "../arena.h"
#include <iostream>
#include <string>

class TestClass
{
public:
	int m_int;
	void print()
	{
		std::cout << "Output: This should print." << std::endl;
	}
	void print2()
	{
		std::cout << "Output: This should also print." << std::endl;
	}
};

class TestClass2
{
public:
	void print()
	{
		std::cout << "Output: This should print as well." << std::endl;
	}
};

class TestClass3
{
public:
	int m_int;
	void print()
	{
		std::cout << "Output: This should print!" << std::endl;
	}
	void print2()
	{
		std::cout << "Output: This should also print." << std::endl;
	}
};

int main()
{
	Arena arena(1000);
	
	// first test
	std::cout << "\nFIRST TEST\nExpected: This should print." << std::endl;
	arena.push(200);
	TestClass* tc = arena.allocate(TestClass());
	tc->print();

	// second test
	std::cout << "\nSECOND TEST\nExpected: This should print.\nExpected: This should also print." << std::endl;
	arena.push(200);
	TestClass* tc2 = arena.allocate(TestClass());
	tc->print();
	tc2->print2();

	// third test
	std::cout << "\nTHIRD TEST\nExpected: This should print." << std::endl;
	arena.pop();
	tc->print();

	// fourth test
	std::cout << "\nFOURTH TEST\nExpected: This should print.\nExpected: This should print as well." << std::endl;
	arena.push(200);
	TestClass2* tc3 = arena.allocate(TestClass2());
	tc->print();
	tc3->print();

	// fifth test
	std::cout << "\nFIFTH TEST\nExpected: not enough space" << std::endl;
	if (!arena.push(800)) {
		std::cout << "Output: not enough space" << std::endl;
	}
	else
	{
		std::cout << "Output: enough space" << std::endl;
	}

	// sixth test
	std::cout << "\nSIXTH TEST\nExpected: too many pops" << std::endl;
	arena.pop();
	arena.pop();
	if (!arena.pop())
	{
		std::cout << "Output: too many pops" << std::endl;
	}
	else
	{
		std::cout << "Output: pop was allowed" << std::endl;
	}

	// seventh test
	std::cout << "\nSEVENTH TEST\nExpected: enough space\nExpected: This should print." << std::endl;
	if (!arena.push(800)) {
		std::cout << "Output: not enough space" << std::endl;
	}
	else
	{
		std::cout << "Output: enough space" << std::endl;
	}
	tc->print();

	// eighth test
	std::cout << "\nEIGHTH TEST\nExpected: enough space\nExpected: This should print!" << std::endl;
	TestClass3* tc4 = arena.allocate(TestClass3());
	tc4->print();


}