#pragma once
#include <mutex>
#include <cstdlib>
#include <new>

/* Simple Memory Arena
   Author: Jun Yen Leung

*  Does NOT support copying
*  Does NOT suport reallocation of previously allocated data (except entire pools)
*  BE SURE TO COMPILE WITH C++ 11+

* Description: 
*  Usage:
*  1. Create an Arena of the desired size (e.g. Arena arena(1000); for one kb)
*  2. Push pools on to the Arena as desired... (e.g. arena.push(200); to push a pool of 200b)
*  3. Allocate memory to your objects as such:
*  		Given an arbitrary class C with constructor C(),
*  			C* c_pointer = arena.allocate(C());
*  4. Pop pools off the Arena as desired... (e.g. arena.pop())
*/


class Arena
{
public:
	Arena(std::size_t size);
	~Arena();
	template <class T> T* allocate(T data); // for user to allocate memory
	bool push(std::size_t size); // push a new pool on to stack
	bool pop(); // pop top pool from stack
private:
	char* m_top_pool = nullptr; // pointer to the start of highest pool
	char* m_start; // pointer to start of arena
	char* m_free; // pointer to first free memory in arena
	char* m_end; // pointer to end of arena
	std::mutex m_mtx;
	struct Pool
	{
		char* prev = nullptr;
		char* next = nullptr;
		size_t size;
		char* pool_start;
		char* pool_free;
		char* pool_end;
	};
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

// pops a pool from the top of the pool stack
bool Arena::pop()
{
	m_mtx.lock();
	if (m_top_pool == nullptr)
	{
		return false;
	}
	// that ugly string of casts is the previous pool
	if (((Pool*)(((Pool*)m_top_pool)->prev)) == nullptr)
	{
		// the last pool on the stack
		// set m_free to m_start
		m_free = m_start;
		// set top pool to null
		m_top_pool = nullptr;
		return true;
	}
	else
	{
		// set previous pool's next pointer to null
		((Pool*)(((Pool*)m_top_pool)->prev))->next = nullptr;
		// set m_free to where the previous pool was
		m_free -= ((Pool*)m_top_pool)->size;
		// set top pool to previous pool
		m_top_pool = (((Pool*)m_top_pool)->prev);
	}
	m_mtx.unlock();
	return true;
}

// pushes a pool to the top of the pool stack
// alignment doesn't matter here because we align within the pools
bool Arena::push(size_t requested_size)
{
	m_mtx.lock();

	size_t pool_size = requested_size + sizeof(Pool);
	size_t total_size = pool_size;

	if (m_end - m_free < pool_size)
	{
		// not enough space
		return false;
	}

	// set last pool's next to this pool, if exists
	if (m_top_pool != nullptr)
	{
		((Pool*)m_top_pool)->next = m_free;
	}

	// metadata at beginning of pool
	Pool new_pool;
	new_pool.prev = m_top_pool;
	new_pool.size = requested_size;
	new_pool.pool_free = m_free + sizeof(Pool);
	new_pool.pool_end = m_free + pool_size;
	new_pool.pool_start = new_pool.pool_free;
	
	// place pool
	*(Pool*)m_free = new_pool;

	// update top pool
	m_top_pool = m_free;

	// increment m_free in arena
	m_free += total_size;

	m_mtx.unlock();
	return true;

}

// allocate with automatic alignment to the top pool
// returns nullptr if not enough space
template <class T> T* Arena::allocate(T data)
{
	m_mtx.lock();
	size_t size = sizeof(data);

	// auto alignment
	size_t alignment = size;

	// align
	size_t buffer = alignment - ((*(Pool*)m_top_pool).pool_free - (*(Pool*)m_top_pool).pool_start) % alignment;

	// if buffer is exactly alignment, we don't need it
	// else, add the buffer
	size_t total_size = (buffer == alignment) ? (size) : (size + buffer);

	if ((*(Pool*)m_top_pool).pool_end - (*(Pool*)m_top_pool).pool_free < total_size)
	{
		// not enough space
		return nullptr;
	}

	char* block_start = (*(Pool*)m_top_pool).pool_free + buffer;
	(*(Pool*)m_top_pool).pool_free += total_size;

	// place data at pointer
	*(T*)block_start = data;

	m_mtx.unlock();
	// return pointer to data
	return (T*)block_start;
}

