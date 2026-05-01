# Port Mapped Input Output


**Port-Mapped I/O (PMIO)** is a communication method where the CPU uses **dedicated, secondary address space** specifically for hardware peripherals, keeping them completely isolated from the systm's main memory (RAM).





## IN / OUT instruction 

`in` and `out` are x86 CPU instruction specifically designed for communicating with hardware devices. `out` sends data to device, `in` reads data from a device. They are the only way the CPU can directly talk to hardware.


## Role of `in` and `out`

The CPU has two completely separate addres spaces -- *memory* and *I/O ports*.
Memory is accessed with normal instruction like `mov`. I/O ports are a dedicated space of 65536 addresses (0x0000-0xFFFF) accessed exclusively through `in` and `out`. Every hardware device is assigned specific port numbers it listens on -- and a device can have **multiple ports**, each serving a different purpose:
```
PIC master  →   0x20    command port    (send actions)
            →   0x21    data port       (configuration)

keyboard    →   0x60    data port       (read scancodes)
            →   0x64    status port     (check if ready)


PIT timer   →   0x40    channel 0 
            →   0x41    channel 1
            →   0x43    mode/command
```


### IN instruction 

```
port    →   must be in DX (or immediate 1-byte value 0x00-0xFF)
result  →   always goes into AL(byte), AX(word), EAX(dword)
```

```
in al,  dx  ;   read byte   →   result in AL 
in ax,  dx  ;   read word   →   result in AX 
in eax, dx  ;   read dword  →   result in EAX 
```

### out instruction 
```
port    →   must be in DX (or immediate 1-byte value 0x00-0xFF)
data    →   must be in AL (byte), AX(word), EAX(dword) 
```
```
out dx, al  ;   send byte   —   port in DX, data in AL 
out dx, xl  ;   send word   —   port in DX, data in XA
out dx, eax ;   send dword  —   port in DX, data in EAX


out ox20, al;   immediate port — only works if port fits in 1 byte
```


### Strict rules 

— **Port**    →   ONLY ***DX*** or ***immediate byte*** no ther register works

— **Data**    →   ONLY ***AL / AX / EAX*** - no  ther register works 

— **Result**  →   ONLY ***AL / AX / EAX*** - no other register works
