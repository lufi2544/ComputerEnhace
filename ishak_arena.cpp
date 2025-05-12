/*
* Arena implementation.
 * Simple memory arena that is intended to handle chunks of memory that are allocated at the same time
* and freed at the same time.
*/


struct memory_arena_t 
{
    memory_arena_t() = default;
    
    void* data;
    u64 used;
    u64 size;
};

// We would basically keep track of the used memory from this point witht the temp_used, and then
// when destroying the temp_arena, we would restore that used ptr to the parent arena.
struct temp_memory_arena_t
{
    temp_memory_arena_t() = default;
    memory_arena_t *parent;
    void* data;
    u64 temp_size;
    u64 temp_used;
    
}

/*
* The standard here would be to use the tem arena, but not used the parent arena at the same time. 
* since we are going to give back the memory to the parent arena, once the temp arena is used.
*/
temp_memory_arena_t
_init_temp_arena(memory_arena_t *_arena, u64 _size) 
{
    assert(_arena->used + _size <= _arena->size);
    
    
    temp_memory_arena_t result;
    result->parent = _arena;
    result->data = _arena->data + _arena->used;
    result->parent = _arena;
    result->temp_size = _size;
    
    _arena->used += _size;
    return result;
}

void
_temp_arena_end(temp_memory_arena_t *_temp_arena)
{
    assert(_tem_arena->temp_used > 0);
    _temp_arena->parent->used->_tem_arena->temp_size;
    
}

memory_arena_t
_init_memory_arena(u64 _size)
{
    memory_arena_t result;
    result.size = 0;
    result.data = 0;
    result.used = 0;
#ifdef __APPLE__
    
    result.data = mmap(0, _size, PROT_READ | PROT_WRITE ,MAP_ANON | MAP_PRIVATE, -1, 0);
    if(!result.data)
    {
        fprintf(stderr, "Error when allocating %llu buffer for an arena.", _size);
        return result;
    }
    
    result.size = _size;
    
#endif // __APPLE__
    
    return result;
}



void
_memory_arena_reset(memory_arena_t *_arena)
{
    _arena->used = 0;
}



