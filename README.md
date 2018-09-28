# raimi_lib

This is a personal C++ library.

I will implement things that I think will be useful for my own programming or are just fun to do.

Wow!

# Currently contains:
Memory arena with pool stack
* This is a pool allocator on top of a stack allocator.
* You alllocate one large "arena" of contiguous memory to begin. All subsequent memory allocations will be from this arena.
* You push "pools" of memory onto the arena as desired. You can only allocate to the most recent pool.
* When you pop, you remove the highest pool from the arena. This memory can now be reused in new pools.
* This allows you to push pools separately for different parts of your program.
* See comments in arena.h for more details.
