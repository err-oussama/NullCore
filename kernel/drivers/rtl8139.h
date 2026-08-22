#ifndef RTL8139_H
#define RTL8139_H

#define RTL8139_COMMAND_OFFSET 0x37 // size: 1byte
// Bit 0:Buffer Empty (BUFE) - RO: flag
// 		0= There is data waiting to be read,
// 		1= RX buffer currently has no unread packets;
//
// Bit 1: Reserved; No Function
//
// Bit 2: Transmitter Enable (TE) - RW: function
// 		0= Transmitter is Disabled and cannot send packets
// 		1= Transmitter is Enabled and can send packets
//
// Bit 3: Receiver Enable (RE) - RW: function
// 		0= Receiver is Disabled and cannot receive packets
// 		1= Receiver is enabled and will accept incoming packets
//
// Bit 4: Reset (RST) - RW, self-clearing: function
//		write 1 triggers a software reset; hardware automatically clears
//    this bit back to 0 once the reset completes
//
// Bit 5-7: Reserved; No function

#endif
