#ifndef RTL8139_H
#define RTL8139_H

// Any bit not explicity defined below is reserved and has no function.

// ############### COMMAND ########################

// Name: COMMAND register
// Size: 1 byte
// Role: Control the behavior of the RTL8139 device
#define RTL8139_CMD_OFFSET 0x37

#define RTL8139_CMD_BUFE 0x1 // Buffer Empty - RO
// Flag: is the RX buffer currently empty (no unread packets waiting)

#define RTL8139_CMD_TE 0x4 // Transmitter Enable - RW
// Enables transimitter to send packets

#define RTL8139_CMD_RE 0x8 // Receiver Enabled - RW
// Enables receiver to accept incoming packets

#define RTL8139_CMD_RST 0x10 // Reset - RW - self-clearing
// write 1 triggers a software reset
// hardware automatically clears this bit back to 0 once the reset completes

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### RCR ########################

// Name: RECEIVE CONFIG REGISTER
// Size: 4 byte
// Role: Register to config the behavior of the receiving mechansim of the
// 			 RTL8139
#define RTL8139_RCR_OFFSET 0x44

#define RTL8139_RCR_AAP 0x1 // Accept All Packets - RW
// Promiscuous mode: accept evert packet regarless of destination address

#define RTL8139_RCR_APM 0x2 // Accept Physical Match - RW
// Accept packets whose destination MAC mathes this devic's own address

#define RTL8139_RCR_AM 0x4 // Accept Multicast - RW
// Accept multicast packets matching the MRA0-7 filter

#define RTL8139_RCR_AB 0x8 // Accept Broadcast - RW
// Accept broadcast packets (destination = FF:FF:FF:FF:FF:FF)

#define RTL8139_RCR_AR 0x10 // Accept Runt - RW
// Accept packets smaller then the standard minimum Ethernet frame size

#define RTL8139_RCR_AER 0x20 // Aeccept Error - RW
// Accept packets that faild RCR/alignment checks

#define RTL8139_RCR_WRAP 0x80 // Wrap
// What happens when the NIC reachs the end of the RX buffer mid-packet:
// 0: wrap to the start of the buffer and continue writing there
// 1: keep writing past the buffer's end (need 1.5K reserved there)
// Invalid when RBLEN is set to 64KB

// Bits 8-10: MXDMA (Max DMA Burst Size per RX DMA Burst) - RW
// Maximum size of a single receive DMA burst transfer
#define RTL8139_RCR_MXDMA_16B 0x00        // 16 Bytes
#define RTL8139_RCR_MXDMA_32B 0x100       // 32 Bytes
#define RTL8139_RCR_MXDMA_64B 0x200       // 64 Bytes
#define RTL8139_RCR_MXDMA_128B 0x300      // 128 Bytes
#define RTL8139_RCR_MXDMA_256B 0x400      // 256 Bytes
#define RTL8139_RCR_MXDMA_512B 0x500      // 512 Bytes
#define RTL8139_RCR_MXDMA_1024B 0x600     // 1024 Bytes
#define RTL8139_RCR_MXDMA_UNLIMITED 0x700 // Unlimited Bytes

// Bits 11-12: RBLEN (RX Buffer Length) - RW
// Size of the RX ring buffer
#define RTL8139_RCR_RBLEN_8KB 0x0000  //  8KB + 16 Byte
#define RTL8139_RCR_RBLEN_16KB 0x0800 // 16KB + 16 Byte
#define RTL8139_RCR_RBLEN_32KB 0x1000 // 32KB + 16 Byte
#define RTL8139_RCR_RBLEN_64KB 0x1800 // 64KB + 16 Byte

// Bits 13-15: RXFTH (RX FIFO Threshold) - RW
// How many bytes of a packet must accumulate in the RX FIFO before the device
// starts transferring them to the host memory (or once a full packet has
// arrived, if sooner)
#define RTL8139_RCR_RXFTH_16B 0x0000   // 16 Bytes
#define RTL8139_RCR_RXFTH_32B 0x2000   // 32 Bytes
#define RTL8139_RCR_RXFTH_64B 0x4000   // 64 Bytes
#define RTL8139_RCR_RXFTH_128B 0x6000  // 128 Bytes
#define RTL8139_RCR_RXFTH_256B 0x8000  // 256 Bytes
#define RTL8139_RCR_RXFTH_512B 0xA000  // 512 Bytes
#define RTL8139_RCR_RXFTH_1024B 0xC000 // 1024 Bytes
#define RTL8139_RCR_RXFTH_NO_TH 0xE000 // No threshold, wait for whole packet

#define RTL8139_RCR_RER8 0x10000 // Receive Error Packet (8-byte threshold) - RW
// 0: Accept error packets only if longer then 64 bytes
// 1: Accept error packets if longer then 8 bytes
// Ignored if AER or AR is set

#define RTL8139_RCR_MULERINT 0x20000 // Multiple Early Interrupt Select - RW
// 0: Only recognized protocols (IP, IPX, NDIS...) trigger early interrupt,
//    based on ERTH
// 1: Any received packet triggers early interrupt, based on MISR settings

// Bits 24-27: ERTH (Early RX Threshold) - RW
// Fraction of the packet that must be transferred to the system buffer before
// an early interrupt fires (in early mode), in 16ths
#define RTL8139_RCR_ERTH_NO_TH 0x0000000 // no early threshold
#define RTL8139_RCR_ERTH_1 0x1000000     // 1/16
#define RTL8139_RCR_ERTH_2 0x2000000     // 2/16
#define RTL8139_RCR_ERTH_3 0x3000000     // 3/16
#define RTL8139_RCR_ERTH_4 0x4000000     // 4/16
#define RTL8139_RCR_ERTH_5 0x5000000     // 5/16
#define RTL8139_RCR_ERTH_6 0x6000000     // 6/16
#define RTL8139_RCR_ERTH_7 0x7000000     // 7/16
#define RTL8139_RCR_ERTH_8 0x8000000     // 8/16
#define RTL8139_RCR_ERTH_9 0x9000000     // 9/16
#define RTL8139_RCR_ERTH_10 0xA000000    // 10/16
#define RTL8139_RCR_ERTH_11 0xB000000    // 11/16
#define RTL8139_RCR_ERTH_12 0xC000000    // 12/16
#define RTL8139_RCR_ERTH_13 0xD000000    // 13/16
#define RTL8139_RCR_ERTH_14 0xE000000    // 14/16
#define RTL8139_RCR_ERTH_15 0xF000000    // 15/16

#endif
