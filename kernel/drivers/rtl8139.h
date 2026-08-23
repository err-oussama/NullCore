#ifndef RTL8139_H
#define RTL8139_H

// Any bit not explicity defined below is reserved and has no function.

// ############### COMMAND ########################

#define RTL8139_CMD_OFFSET 0x37 // COMMAND register, size: 1 byte

#define RTL8139_CMD_BUFE 0x1 // Buffer Empty - RO
// Flag: is the RX buffer currently empty (no unread packets waiting)

#define RTL8139_CMD_TE 0x4 // Transmitter Enable - RW
// Enables transimitter to send packets

#define RTL8139_CMD_RE 0x8 // Receiver Enabled - RW
// Enables receiver to accept incoming packets

#define RTL8139_CMD_RST 0x10 // Reset - RW - self-clearing
// write 1 triggers a software reset
// hardware automatically clears this bit back to 0 once the reset completes

#endif
