#pragma once
#include <mutex>
#include <cstdlib>
#include <new>

/* Simple Memory Arena 

*  Does NOT support copying
*  Does NOT suport reallocation of previously allocated data
*  BE SURE TO COMPILE WITH C++ 11+

*  Usage:
*  In your program...
*  1. #include "arena.h"
*  2. Create an Arena of the desired size (e.g. Arena arena(1000); for one kb)
*  3. Create your objects as such:
*  		Given an arbitrary class C with constructor C(),
*  			C* c_pointer = arena.allocate(C());
*  4. Delete your arena object if you dynamically allocated it. 
*/

class Arena
{
public:
	Arena(std::size_t size);
	~Arena();
	template <class T> T* allocate(T data);
private:
	char* m_start; // pointer to start of arena
	char* m_free; // pointer to first free memory in arena
	char* m_end; // pointer to end of arena
	std::mutex m_mtx;
};

Arena::Arena(std::size_t size)
{
	m_mtx.lock();
	
	m_start = static_cast<char*>(::operator new(size));
	m_free = m_start;
	m_end = m_start + size;

	m_mtx.unlock();
}

Arena::~Arena()
{
	m_mtx.lock();
	::operator delete(m_start);
	m_mtx.unlock();
}

// allocate with automatic alignment
// returns nullptr if not enough space
template <class T> T* Arena::allocate(T data)
{
	m_mtx.lock();
	
	size_t size = sizeof(data);

	// auto alignment
	size_t alignment = size;

	// align
	size_t buffer = alignment - (m_free - m_start) % alignment;

	// if buffer is exactly alignment, we don't need it
	// else, add the buffer
	size_t total_size = (buffer == alignment) ? (size) : (size + buffer);

	if (m_end - m_free < total_size)
	{
		// not enough space
		return nullptr;
	}

	char* block_start = m_free + buffer;
	m_free += total_size;

	// place data at pointer
	*(T*)block_start = data;

	m_mtx.unlock();

	// return pointer to data
	return (T*)block_start;
}

