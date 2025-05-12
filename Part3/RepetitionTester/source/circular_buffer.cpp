//

#ifdef __APPLE__
#include <sys/mman.h>
#include <fcntl.h>
#endif //__APPLE__

void *init_circular_buffer_test(int size, int *pages)
{
#if defined (__APPLE__)
    // I will have to use the shm_open in macOS to create a shared memory object and then map the circular buffer to this object.
    // - Add protection for creation between processes, memory should be private
    
    u8 array_num = 24;
    int per_page = getpagesize() / sizeof(u8);
    printf("final: %i \n", per_page);
    
    
    
    int fd = shm_open("/myshm", O_CREAT | O_RDWR, 0600);
    
    int pages_num = size / getpagesize();
    if(size % getpagesize() != 0)
    {
        pages_num++;
    }
    *pages = pages_num;
    printf("pages: %i \n", pages_num);
    int to_map_size = getpagesize();
    ftruncate(fd, to_map_size);
    
    u8 *base = (u8*)mmap(NULL, pages_num * getpagesize(), PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
    for (int i = 0; i < pages_num; ++i)
    {
        mmap(base + (getpagesize() * i ), to_map_size, PROT_READ | PROT_WRITE, MAP_FIXED | MAP_SHARED, fd, 0);
    }
    
    return base;
#else 
#endif // PLATFORMS
	
	return 0;
}