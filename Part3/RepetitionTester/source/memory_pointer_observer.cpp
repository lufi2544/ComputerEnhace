
typedef struct 
{
    u16 pml4_index;
    u16 directoryptr_index;
    u16 directory_index;
    u16 table_index;
    u16 offset;
    
} address_info_t;


internal_f void 
print_binary_bits(u64 _value, u32 _first_bit, u32 _bit_count)
{
    for (u32 bit_index = 0; bit_index < _bit_count; ++bit_index)
    {
        u64 Bit = (_value >> ((_bit_count - 1 - bit_index) + _first_bit)) & 1;
        printf("%c", Bit ? '1' : '0');
    }
}

internal_f address_info_t
decompose_ptr(void* ptr)
{
    address_info_t result;
    
#ifdef _WIN32
    result.pml4_index = (((u64)ptr >> 39) & 0x1ff);
    result.directoryptr_index = (((u64)ptr >> 30) & 0x1ff);
    result.directory_index = (((u64)ptr >> 21) & 0x1ff);
    result.table_index = (((u64)ptr >> 12) & 0x1ff);
    result.offset = (((u64)ptr) & 0xfff);
#endif // _WIN32
    
#ifdef __APPLE__
    
    result.pml4_index = (((u64)ptr >> 47) & 0x1);
    result.directoryptr_index = (((u64)ptr >> 36) & 0x7ff);
    result.directory_index = (((u64)ptr >> 25) & 0x7ff);
    result.table_index = (((u64)ptr >> 14) & 0x7ff);
    result.offset = (((u64)ptr >> 0) & 0x3fff);
    
#endif // __APPLE__
    
    
    
    return result;
}

internal_f void
print_address(void* _ptr)
{
    u64 address = (u64)_ptr;
    printf("Address: %llu ", address);
    print_binary_bits(address, 48, 16);
    printf("|");
    print_binary_bits(address, 47, 1);
    printf("|");
    print_binary_bits(address, 36, 11);
    printf("|");
    print_binary_bits(address, 25, 11);
    printf("|");
    print_binary_bits(address, 14, 11);
    printf("|");
    print_binary_bits(address, 0, 14);
    printf("\n");
    
    address_info_t address_info = decompose_ptr(_ptr);
    
    printf(" %3u | %3u | %3u | %3u | %u \n", address_info.pml4_index, address_info.directoryptr_index, address_info.directory_index, address_info.table_index, address_info.offset);
}


internal_f void* 
memory_ptr_test()
{
    //for (int idx = 0; idx < 16; ++idx)
    
    u8 page_id = 2;
    u8 offset_within_page = 1;
    void *data = mmap(0, 1024*1024, PROT_READ | PROT_WRITE ,MAP_ANON | MAP_PRIVATE, -1, 0);
    print_address(data);
    u8* modified_data = (u8*)data + ((16*1024 * page_id) + sizeof(u8) * offset_within_page);
    
    *modified_data = 255;
    
    print_address(modified_data);
    
    return data;
    
}
