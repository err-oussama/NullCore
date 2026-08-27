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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### TCR ########################

// Name: TRANSMIT CONFIG REGISTER
// Size: 4 byte
// Role: Register to config the behavior of the transmitting mechaanism of the
// 			 RTL8139

#define RTL8139_TCR_OFFSET 0x40

#define RTL8139_TCR_CLRABT 0x1 // Clear Abort - W
// Write 1 to retry the last transmitted (aborted) packet;
// only valid while the device is in the transmit abort state

// Bits 4-7: TXRR (TX Retry Count) - RW
// Number of retransmit attempts after a collision, in multiples of 16
// Total retries = 16 + (TXRR * 16); if TXRR = 0, retries 16 times before
// aborting with excessive collisions. TER (in ISR) is set once this limit is
// reached
#define RTL8139_TCR_TXRR0 0x00  // 16 Retry
#define RTL8139_TCR_TXRR1 0x10  // 32 Retry
#define RTL8139_TCR_TXRR2 0x20  // 48 Retry
#define RTL8139_TCR_TXRR3 0x30  // 64 Retry
#define RTL8139_TCR_TXRR4 0x40  // 80 Retry
#define RTL8139_TCR_TXRR5 0x50  // 96 Retry
#define RTL8139_TCR_TXRR6 0x60  // 112 Retry
#define RTL8139_TCR_TXRR7 0x70  // 128 Retry
#define RTL8139_TCR_TXRR8 0x80  // 144 Retry
#define RTL8139_TCR_TXRR9 0x90  // 160 Retry
#define RTL8139_TCR_TXRR10 0xA0 // 176 Retry
#define RTL8139_TCR_TXRR11 0xB0 // 192 Retry
#define RTL8139_TCR_TXRR12 0xC0 // 208 Retry
#define RTL8139_TCR_TXRR13 0xD0 // 224 Retry
#define RTL8139_TCR_TXRR14 0xE0 // 240 Retry
#define RTL8139_TCR_TXRR15 0xF0 // 256 Retry

// Bits 8-10: MXDMA (Max DMA Burst Size per TX DMA Burst) - RW
// Maximum size of a single transmit DMA burst transfer
#define RTL8139_TCR_MXDMA16 0x000   // 16 Bytes
#define RTL8139_TCR_MXDMA32 0x100   // 32 Bytes
#define RTL8139_TCR_MXDMA64 0x200   // 64 Bytes
#define RTL8139_TCR_MXDMA128 0x300  // 128 Bytes
#define RTL8139_TCR_MXDMA256 0x400  // 256 Bytes
#define RTL8139_TCR_MXDMA512 0x500  // 512 Bytes
#define RTL8139_TCR_MXDMA1024 0x600 // 1024 Bytes
#define RTL8139_TCR_MXDMA2048 0x700 // 2048 Bytes

#define RTL8139_TCR_CRC 0x10000 // Append CRC - RW
// 0: CRC is appended to the end of the packet
// 1: No CRC is appended

// Bits 17-18: LBK (LoopBack Test) - RW
// 00: Normal operation
// 01: Reserved
// 11: Loopback mode (no packets appear on TX+/-)
#define RTL8139_TCR_LBK_NORMAL 0x00000
#define RTL8139_TCR_LBK_LOOPBACK 0x60000

// Bits 22-23: HWVERID_B (Hardware Version ID) - RO

// Bits 24-25: IFG (InterFrame Gap Time) - RW
// Adjust the gap between frames below the IEEE 802.3 standard
// 10 Mbps : 8.4us + 0.4 * IFG us
// 100 Mbps: 840ns + 40 * IFG us
// Only (1,1) is IEEE 802.3 compliant; any other value violates the standard
#define RTL8139_TCR_IFG 0x3000000 // (1,1) - complaint

// Bits 26-30: HWVERID_A (Hardware Version ID) - RO
// Identifies the specific RTL8139/8100 chip varient; see datasheet table

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### ISR ########################

// Name: Interrupt Status Register
// Size: 2 byte
// Role: Reports which interrupt condition(s) triggered.
// 			 Reading it clears every set bit (clear-on-read).
// 			 Write has no effect.
// 			 Bits still get set here even if masked in IMR -
// 			 the mask only controls whether a set bit actually raise
// 			 a hardware interrupt.

#define RTL8139_ISR_OFFSET 0x3E

#define RTL8139_ISR_ROK 0x1 // Receive (RX) OK - RO
// A packet was received successfully (normal mode), or the arriving
// packet's byte count exceeded the early Rx threshold (early mode)

#define RTL8139_ISR_RER 0x2 // Receive (RX) Error - RO
// The received packet had a CRC error or frame alignment error
// (collided frames shorter then 16 bytes are not flagged as CRC errors)

#define RTL8139_ISR_TOK 0x4 // Transmit (TX) OK - RO
// A packet transmission completed successfully

#define RTL8139_ISR_TER 0x8 // Transmit (TX) Error - RO
// A packet transmission was aborted due to excessive collisions,
// per the TXRR retry-count setting

#define RTL8139_ISR_RXOVW 0x10 // RX Buffer Overflow - RO
// RX buffer ring storage has been exhausted

#define RTL8139_ISR_PUN_LINKCHG 0x20 // Packet Underran / Link Change - RO
// Set when CAPR is written while the RX buffer is empty,
// or when the link status changes

#define RTL8139_ISR_FOVW 0x40 // RX FIFO Overflow - RO
// An overflow occurred on the Rx status FIFO

#define RTL8139_ISR_LENCHG 0x2000 // Cable Length Change - RO
// Cable length changed after the receiver was enbaled

#define RTL8139_ISR_TIMEOUT 0x4000 // Time Out - RO
// TCTR register reached the value in the TimerInt register

#define RTL8139_ISR_SERR 0x8000 // System Error - RO
// The device signaled a system error on the PCI bus

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### IMR ########################

// Name: Interrupt Mask Register
// Size: 2 byte
// Role: Select which ISR conditions are allowed to raise a hardware
// 			 interrupt (INTA#).
//       A bit set here does not affect whether ISR's corresponding bit
//       get set - only whether it triggers an interrupt.
//       Cleared entirely by a hardware reset, so must be explicitly
//       configured during init.
//       1: that condition will trigger an interrupt, 0: it will not

#define RTL8139_IMR_OFFSET 0x3C
#define RTL8139_IMR_ROK 0x1          // Receive (RX) OK - RW
#define RTL8139_IMR_RER 0x2          // Receive (RX) Error - RW
#define RTL8139_IMR_TOK 0x4          // Transmit (TX) OK - RW
#define RTL8139_IMR_TER 0x8          // Transmit (TX) Error - RW
#define RTL8139_IMR_RXOVW 0x10       // RX Buffer Overflow - RW
#define RTL8139_IMR_PUN_LINKCHG 0x20 // Packet Underrun / Link  Change - RW
#define RTL8139_IMR_FOVW 0x40        // RX FIFO Overflow - RW
#define RTL8139_IMR_LENCHG 0x2000    // Cable Length Change - RW
#define RTL8139_IMR_TIMEOUT 0x4000   // Time Out - RW
#define RTL8139_IMR_SERR 0x8000      // System Error - RW

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### CONFIG1 ########################

// Name: Configuration Register 1
// Size: 1 byte
// Role: Miscellaneous device configuration - LED pins, wake pin behavior,
//       power management capabilty, and driver-load flag.

#define RTL8139_CFG1_OFFSET 0x52

#define RTL8139_CFG1_PMEn 0x1 // Power Management Enable - WR
// Only writable when 93C46CR EEM1=EEM0=1; exposese/hides the PCI power
// management capability structure at config space offset 0x50-0x57
//
#define RTL8139_CFG1_VPD 0x2 // Vital Product Data - WR
// Enable VPD data stored in the 93C46 EEPROM at offset 0x40-0x7F
//
#define RTL8139_CFG1_IOMAP 0x4 // I/O Mapping - RO
// 1 = operational registers are mapped into PCI I/O space (BAR0)
//
#define RTL8139_CFG1_MEMMAP 0x8 // Memory Mapping - RO
// 1 = operational registers are mapped into memory space (BAR1)
//
#define RTL8139_CFG1_LWACT 0x10 // LWAKE active mode - RW
// Selects the LWAKE pin's output signal polarity/pulse type, combined with
// CONIFG4's LWPIN bit; irrelevent without real wake-on-LAN hardware

#define RTL8139_CFG1_DVRLOAD 0x20 // Driver Load - RW
// Software sets this to mark that a driver has loaded. automatically
// cleared by hardware once Command register IOEN/MEMEN/BMEN are written

// Bits 6-7: LEDS1-0 - RW
// Controls physical LET pin behavior; initial value comes from 93C46
#define RTL8139_CFG1_LEDS1_00 0x00
#define RTL8139_CFG1_LEDS1_01 0x40
#define RTL8139_CFG1_LEDS1_10 0x80
#define RTL8139_CFG1_LEDS1_11 0xC0

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### RBSTART ########################

// Name: Receive (RX) Buffer Address Start
// Size: 4 byte
// Role: Physical address of the RX ring buffer.
//
#define RTL8139_RBSTART_OFFSET 0x30

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### TSAD ########################

// Name: Transmit Start Address of Descriptor 0-3
// Size: 4 byte
// Role: Physical address of the buffer holding the packet to transmit for
//       the corresponding TX slot.
//       Write this before triggering send via the matching TSD register.
#define RTL8139_TSAD0_OFFSET 0x20
#define RTL8139_TSAD1_OFFSET 0x24
#define RTL8139_TSAD2_OFFSET 0x28
#define RTL8139_TSAD3_OFFSET 0x2C

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// ############### TSD ########################

// Name: Transmit Status Descriptor 0-3
// Size: 4 byte
// Role: Triggers transmission and reports status for the corresponding
//       TX slot.
//       Writting the SIZE field (bits 0-12) clears all read-only status
//       bits and begins transmission.
//       After a software reset, every bit except OWN is cleared.
//       OWN defaults to 1.
#define RTL8139_TSD0_OFFSET 0x10
#define RTL8139_TSD1_OFFSET 0x14
#define RTL8139_TSD2_OFFSET 0x18
#define RTL8139_TSD3_OFFSET 0x1C

// Bits 0-12: SIZE - RW
// Total size in bytes of this descriptor's packet data.
// Writing this field triggers transmission.
// Max 1792 byte (0x700) per descriptor - exceeding this invalidates the
// TX queue until OWN is set again

#define RTL8139_TSD_OWN 0x2000 // OWN - RW
// 1: (default) TX DMA for this descriptor completed / slot is free
// 0: descriptor is in use; software must clear this when writing SIZE
//    to hand the descriptor to the device

#define RTL8139_TSD_TUN 0x4000 // Transmit FIFO Underrun - RO
// Set if the TX FIFO was exhusted during transmission.
// The packet may still have been send successfully despite this
// (check TOK/ISR<TER>)
//
//
#define RTL8139_TSD_TOK 0x8000 // Transmit OK - RO
// Set when the packet transmitted successfully with no FIFO underrun

// Bits 16-21: ERTXTH (Early TX Threshold) - RW
// TX FIFO fill level (in 32-bit units, except 0 = 8 byte) that triggers
// transmission to begin, similar to RXFTH on the receive side.
// Must no exceed 2K bytes.
// value N (1-63) = N * 32 byte

// Bits 24-27: NCC (Number of Collision Count) - RO
// Number of collisions encountered while transmitting this packet

#define RTL8139_TSD_CDH 0x10000000 // CD Heard Beat - RO
// Set if the transceiver faild to send the CD Heartbeat signal during
// the interframe gap.
// Always 0 in 100 Mbps mode.

#define RTL8139_TSD_OWC 0x20000000 // Out of Window Collision - RO
// Set if an `out of window` collision occurred during transmission

#define RTL8139_TSD_TABT 0x40000000 // Transmit Abort - RO
// Set if transmission of the packet was aborted

#define RTL8139_TSD_CRS 0x80000000 // Carrier Sense Lost - RO
// Set if carrier was lost during transmission

#endif
