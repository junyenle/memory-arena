#include "../arena.h"
#include <iostream>
#include <string>

class TestClass
{
public:
	int m_int;
	void print()
	{
		std::cout << "This should print." << std::endl;
	}
};

int main()
{
	/* ARENA ON THE STACK */
	std::cout << "ARENA ON THE STACK TESTS" << std::endl;
	Arena arena(1000);

	// built in data / objects
	int* int1 = arena.allocate(5);
	std::string* str = arena.allocate(std::string("hi"));
	std::cout << "Should output: 5     Outputs: " << *int1 << std::endl;
	std::cout << "Should output: hi    Outputs: " << *str << std::endl;

	// custom objects
	// data
	TestClass* testClass = arena.allocate(TestClass());
	testClass->m_int = 10;
	std::cout << "Should output: 10    Outputs: " << testClass->m_int << std::endl;
	// calling functions
	testClass->print();


	/* ARENA ON THE HEAP */
	std::cout << "\nARENA ON THE HEAP TESTS" << std::endl;
	Arena* arena_ptr = new Arena(1000);

	// built in data / objects
	int* int2 = arena_ptr->allocate(5);
	std::string* str2 = arena_ptr->allocate(std::string("hi"));
	std::cout << "Should output: 5     Outputs: " << *int2 << std::endl;
	std::cout << "Should output: hi    Outputs: " << *str2 << std::endl;

	// custom objects
	// data
	TestClass* testClass2 = arena_ptr->allocate(TestClass());
	testClass2->m_int = 10;
	std::cout << "Should output: 10    Outputs: " << testClass2->m_int << std::endl;
	// calling functions
	testClass2->print();

	// cleanup
	delete arena_ptr;

}