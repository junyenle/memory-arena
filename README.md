# Memory arena
* This is a custom memory allocator / manager.
* I say manager but the management is very simple because this way is fast.
* You alllocate one large "arena" of contiguous memory to begin. All subsequent memory allocations will be from this arena.
* You push blocks of memory onto the arena as desired. You can only allocate to the most recent block.
* When you pop, you remove the highest block from the arena. This memory can now be reused in new block.
* This allows you to push block separately for different parts of your program.
* See comments in arena.h for more details.
