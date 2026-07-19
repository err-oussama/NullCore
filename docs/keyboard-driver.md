# IO_DEVICE


## KEYBOARD


### SCANCODE

A **scancode** is a single byte that the keyboard sends to the CPU identify which physical key was pressed or released. It has nothing to do with charachters ASCII -- it only identifies the physical position of the key on the keyboard.


Every key on the keyboard has a unique scancode assigned to it permanently -- the `A` key is always `0x1E`, the `S` is always `0x1F`, regardless of language, layout, or what charachter that key is supposed to produce. The scancode never change -- it is tied of the physical key position, not the charachter on the keycap

Being sinlge byte, the scancode uses its bits like this:

```
bits 0-6    →   which physical key

bit 7       →   0 = key pressed  (make code)
                1 = key released (break code)
```

This is the fundamental distinction:

```
scancode    →   which physical key was touched
ASCII       →   what charachter that key should produce 
```

The same physical key with scancode `0x1E` produces:

```
'a' →   nromally
'A' →   when shift is held
```

Same scancode, different charachter -- the scancode is always `0x1E` but what you do with it deponds on the keyboard state. Your driver is responsible for that translation -- the keyboard hardware only every tells you which key moved, never what charachter to produce.

|*Scancode* |    *Key*  |   |*Scancode* |       *Key*       |
|-----------|-----------|---|-----------|-------------------|
|0x00       |Reserved   |   |0x30       |B                  |
|0x01       |Escape     |   |0x31       |N                  |
|0x02       |1          |   |0x32       |M                  |
|0x03       |2          |   |0x33       |,                  |
|0x04       |3          |   |0x34       |.                  |
|0x05       |4          |   |0x35       |/                  |
|0x06       |5          |   |0x36       |Right Shift        |
|0x07       |6          |   |0x37       |Numpad *           |
|0x08       |7          |   |0x38       |Left Alt           |
|0x09       |8          |   |0x39       |Space              |
|0x0A       |9          |   |0x3A       |Caps Lock          |
|0x0B       |0          |   |0x3B       |F1                 |
|0x0C       |-          |   |0x3C       |F2                 |
|0x0D       |=          |   |0x3D       |F3                 |
|0x0E       |Backspace  |   |0x3E       |F4                 |
|0x0F       |Tab        |   |0x3F       |F5                 |
|0x10       |Q          |   |0x40       |F6                 |
|0x11       |W          |   |0x41       |F7                 |
|0x12       |E          |   |0x42       |F8                 |
|0x13       |R          |   |0x43       |F9                 |
|0x14       |T          |   |0x44       |F10                |
|0x15       |Y          |   |0x45       |Num Lock           |
|0x16       |U          |   |0x46       |Scroll Lock        |
|0x17       |I          |   |0x47       |Numpad 7 / Home    |
|0x18       |O          |   |0x48       |Numpad 8 / Up      |
|0x19       |P          |   |0x49       |Numpad 9 / PgUp    |
|0x1A       |[          |   |0x4A       |Numpad -           |
|0x1B       |]          |   |0x4B       |Numpad 4 / Left    |
|0x1C       |Enter      |   |0x4C       |Numpad 5           |
|0x1D       |Left Ctrl  |   |0x4D       |Numpad 6 / Right   |
|0x1E       |A          |   |0x4E       |Numpad +           |
|0x1F       |S          |   |0x4F       |Numpad 1 / End     |
|0x20       |D          |   |0x50       |Numpad 2 / Down    |
|0x21       |F          |   |0x51       |Numpad 3 / PgDn    |
|0x22       |G          |   |0x52       |Numpad 0 / Ins     |
|0x23       |H          |   |0x53       |Numpad . / Del     |
|0x24       |J          |   |0x54       |Undefine           |
|0x25       |K          |   |0x55       |Undefine           |
|0x26       |L          |   |0x56       |Undefine           |
|0x27       |;          |   |0x57       |F11                |
|0x28       |'          |   |0x58       |F12                |
|0x29       |`          |   |||
|0x2A       |Left Shift |   |||
|0x2B       |\          |   |||
|0x2C       |Z          |   |||
|0x2D       |X          |   |||
|0x2E       |C          |   |||
|0x2F       |V          |   |||




