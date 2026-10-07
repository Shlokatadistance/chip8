I am coming here primarily from python, so theres a lot to unpack here. 

In python, every object lives on the heap. Because of its vastness, you actually never bother about allocation size for the most part. 

But a buffer that is 4096 bytes in c++ can be issue if its on the stack, especially when you're talking about a machine almost 40 years old. 