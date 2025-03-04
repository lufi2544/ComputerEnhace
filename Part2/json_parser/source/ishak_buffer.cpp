


//--------------------------
// -------- Buffer ---------
//--------------------------

#define CONSTANT_STRING(String) {sizeof(String) - 1, (u8*)(String)}

struct buffer
{
    u8* Bytes;
    u64 Size;
};


function_global buffer
AllocateBuffer(s64 Size)
{
    buffer Result = {};    
    u8* Bytes  = (u8*)malloc(sizeof(u8) * Size + 1);
    memset(Bytes,'\0', Size + 1);
    
    Result.Bytes = Bytes;
    Result.Size = Size;
    
    return Result;
}

function_global void
ResetBuffer(buffer *Buffer)
{
    memset(Buffer->Bytes, '\0', Buffer->Size);    
    Buffer->Size = 0;
}

function_global buffer
AllocateBufferCopyAsString(buffer *Buffer)
{    
    buffer BufferCopy = AllocateBuffer(Buffer->Size);
    memcpy(BufferCopy.Bytes, Buffer->Bytes, Buffer->Size);
    //snprintf(CategoryKeyBuffer, BufferSize, "%s", CategoryContext->Key);
    
    // The targeted JsonValue will ownn this Buffer and will be responsible of releasing it from memory.
    return BufferCopy;
}

function_global void 
FreeBuffer(buffer *Buffer)
{
    if(Buffer->Bytes)
    {
        free(Buffer->Bytes);
    }   
}
