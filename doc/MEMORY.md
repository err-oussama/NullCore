# Memory


## Frame

A **Frame** is a **fixed-siz 4KB of physical RAM**. It is the smallest unit the physical memory manager works with. Physical memory is divided into frames sequentially from address 0 to the top of RAM -- frame 0 starts at address `0x00000000`, frame 1 at `0x00001000`, frame 2 at `0x00002000`, and so on. When the physical memory manager allocates memory it hands out one frame at a time, and when it frees memory it takes one frame back.


## Page
