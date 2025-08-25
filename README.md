# RtlHijack
A repository with scripts to demonstrate alternative Read and Write primitives using Rtl* functions the unintended way.

## RtlFillMemory.c  

`RtlFillMemory` takes three arguments: a pointer to the destination memory, the number of bytes to write, and a single byte value. Writes only a byte into the specified memory block.

## RtlInitializeBitMapEx.c  

Writing primitive using a combination of `RtlInitializeBitMapEx` and `RtlFillMemory`.  
First, it loops over the string in 16-byte chunks, splitting each block into two 8-byte values.  The first 8-byte block is stored in the first argument of `RtlInitializeBitMapEx` and the second 8-byte block is stored into the second argument. 
Then, if an extra 8-byte block remains, it writes that using `RtlInitializeBitMapEx` with a NULL second parameter.  
Finally, if fewer than 8 bytes remain, it writes them one byte at a time using `RtlFillMemory` until the entire string is copied into the destination buffer.

## RtlQueryDepthSList.c

Reading primitive using `RtlQueryDepthSList`. Useful for ROP Gadget hunting, byte comparison, hook detection, etc. Here I demonstrate a simple process injection with ROP Gadget. `RtlQueryDepthSList` accepts a single argument—the memory address to read from, but it only reads 2 bytes and returns a 4-byte value.

## Credits
- [trickster0](https://github.com/trickster0) for his awesome [research](https://trickster0.github.io/posts/Primitive-Injection/).
