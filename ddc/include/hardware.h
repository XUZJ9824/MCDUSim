/******************************************************************************
 * FILE: hardware.h
 *
 * DESCRIPTION:
 *
 * Prototypes for RTL level device register and memory access
 * routines, and register, memory, and mask name associations.
 *
 * Data Device Corporation
 * 105 Wilbur Place
 * Bohemia N.Y. 11716
 * (631) 567-5600
 *
 * Copyright (c) 2007 by Data Device Corporation
 * All Rights Reserved.
 *****************************************************************************/

#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#include "deviceop.h"

/* Multi-IO registers */
#define MIO_FLASH_ADDR                 0x400
#define MIO_FLASH_IN                   0x401
#define MIO_FLASH_OUT                  0x402
#define MIO_CAPABILITIES               0x404
#define MIO_DATA_ARCHIVE               0x405
#define MIO_FPGA_REV                   0x406
#define MIO_FLASH_START                0x409
#define MIO_FLASH_CLUSTERS             0x40A
#define MIO_FLASH_CLUSTERS_SIZE        0x40B
#define MIO_IRIG_CFG_A                 0x40C
#define MIO_TEST_VAL                   0x40D
#define MIO_IRIG_CFG_B                 0x40D
#define MIO_INT_STATUS                 0x414
#define MIO_DISC                       0x41C
#define MIO_AVION                      0x41D
#define MIO_LED_CTRL                   0x425
#define MIO_LED_DATA                   0x426
#define MIO_TEST_MUX_A                 0x428
#define MIO_TT_LSB                     0x43B
#define MIO_TT_MSB                     0x43C

#define MIO_429_RESET                  0x538

/* Card reset and interrupt enable masks. */
#define CARD_RESET                     0x00000001

/* Interrupt enable masks. */
#define ARINC_INT_ENABLE               0x00000002
#define SER_INT_ENABLE                 0x80000000

#define MIO_429_TX_CTRL_0              0x530
#define MIO_429_TX_CTRL_1              0x531
#define MIO_429_TX_CTRL_2              0x532
#define MIO_429_TX_CTRL_3              0x533
#define MIO_429_TX_CTRL_4              0x534
#define MIO_429_TX_CTRL_5              0x535
#define MIO_429_TX_CTRL_6              0x536
#define MIO_429_TX_CTRL_7              0x537

#define MIO_429_RX_CTRL_0              0x502
#define MIO_429_RX_CTRL_1              0x505
#define MIO_429_RX_CTRL_2              0x508
#define MIO_429_RX_CTRL_3              0x50B
#define MIO_429_RX_CTRL_4              0x50E
#define MIO_429_RX_CTRL_5              0x511
#define MIO_429_RX_CTRL_6              0x514
#define MIO_429_RX_CTRL_7              0x517
#define MIO_429_RX_CTRL_8              0x51A
#define MIO_429_RX_CTRL_9              0x51D
#define MIO_429_RX_CTRL_10             0x520
#define MIO_429_RX_CTRL_11             0x523
#define MIO_429_RX_CTRL_12             0x526
#define MIO_429_RX_CTRL_13             0x529
#define MIO_429_RX_CTRL_14             0x52C
#define MIO_429_RX_CTRL_15             0x52F

#define MIO_429_RX_INTEN_0             0x500
#define MIO_429_RX_INTEN_1             0x503
#define MIO_429_RX_INTEN_2             0x506
#define MIO_429_RX_INTEN_3             0x509
#define MIO_429_RX_INTEN_4             0x50C
#define MIO_429_RX_INTEN_5             0x50F
#define MIO_429_RX_INTEN_6             0x512
#define MIO_429_RX_INTEN_7             0x515
#define MIO_429_RX_INTEN_8             0x518
#define MIO_429_RX_INTEN_9             0x51B
#define MIO_429_RX_INTEN_10            0x51E
#define MIO_429_RX_INTEN_11            0x521
#define MIO_429_RX_INTEN_12            0x524
#define MIO_429_RX_INTEN_13            0x527
#define MIO_429_RX_INTEN_14            0x52A
#define MIO_429_RX_INTEN_15            0x52D

#define MIO_429_RX_INTSTAT_0           0x501
#define MIO_429_RX_INTSTAT_1           0x504
#define MIO_429_RX_INTSTAT_2           0x507
#define MIO_429_RX_INTSTAT_3           0x50A
#define MIO_429_RX_INTSTAT_4           0x50D
#define MIO_429_RX_INTSTAT_5           0x510
#define MIO_429_RX_INTSTAT_6           0x513
#define MIO_429_RX_INTSTAT_7           0x516
#define MIO_429_RX_INTSTAT_8           0x519
#define MIO_429_RX_INTSTAT_9           0x51C
#define MIO_429_RX_INTSTAT_10          0x51F
#define MIO_429_RX_INTSTAT_11          0x522
#define MIO_429_RX_INTSTAT_12          0x525
#define MIO_429_RX_INTSTAT_13          0x528
#define MIO_429_RX_INTSTAT_14          0x52B
#define MIO_429_RX_INTSTAT_15          0x52E

#define MIO_429_MAX_NUM_RX             16
#define MIO_429_MAX_NUM_TX             8
#define MIO_429_MAX_NUM_GROUP          8
#define MIO_429_MAX_NUM_DISC           16

/* Serial UART control registers */
#define MIO_UART_1                     0x700
#define MIO_UART_2                     0x710
#define MIO_UART_3                     0x720
#define MIO_UART_4                     0x730
#define MIO_UART_IO_CTRL               0x790

/* Scheduler registers */
#define MIO_SCH_RESOL                  0x800
#define MIO_SCH_CHAN_SEL               0x801
#define MIO_SCH_AUTO_ENA               0x804
#define MIO_SCH_AUTO_PER               0x805
#define MIO_SCH_AUTO_GAP               0x806
#define MIO_SCH_TX_FIFO                0x807
#define MIO_SCH_CHAN_DIS               0x808
#define MIO_SCH_STOP                   0x809
#define MIO_SCH_FIFO_DIS               0x80A
#define MIO_SCH_Q_LEN                  0x80B
#define MIO_SCH_FIFO_STAT              0x803
#define MIO_429_TX_FIFO_STAT_0  	   0x804
#define MIO_429_TX_FIFO_STAT_1         0x805
#define MIO_429_TX_FIFO_STAT_2  	   0x806
#define MIO_429_TX_FIFO_STAT_3         0x807
#define MIO_429_TX_FIFO_STAT_4  	   0x808
#define MIO_429_TX_FIFO_STAT_5         0x809
#define MIO_429_TX_FIFO_STAT_6  	   0x80A
#define MIO_429_TX_FIFO_STAT_7         0x80B

#define MIO_429_TX_FIFO_EMPTY_MASK     0x00000001
#define MIO_429_TX_FIFO_FULL_MASK      0x00000002
#define MIO_429_TX_FIFO_COUNT_MASK     0x03FF0000

#define MIO_SCH_RESOL_1MS              40000
#define MIO_SCH_FIFO_LEN               16
#define MIO_TX_FIFOMB_LEN              512

/* Scheduler memory */
#define MIO_SCH_MSG_DATA               0x00204000
#define MIO_SCH_Q_ACTIVE               0x00204400
#define MIO_SCH_Q_CHAN                 0x00204600
#define MIO_SCH_Q_PTR                  0x00204800
#define MIO_SCH_Q_FREQ                 0x00204A00
#define MIO_SCH_Q_OFST                 0x00204C00
#define MIO_SCH_Q_RPT                  0x00204E00

#define MIO_SCH_MSG_MEM_DEPTH          0x00000400
#define MIO_SCH_Q_DEPTH                0x00000040

/* Masks */
#define MIO_INT_STATUS_TTRO            0x00004000
#define MIO_INT_STATUS_429             0x00000040
#define MIO_INT_STATUS_SER             0x00000080
#define MIO_CHAN_CNT_1553M             0x02000000
#define MIO_CHAN_CNT_1553M_SHFT        22
#define MIO_CHAN_CNT_1553L             0x00000007
#define MIO_CHAN_CNT_429RX             0x000000F8
#define MIO_CHAN_CNT_429RX_SHFT        3
#define MIO_CHAN_CNT_429TXL            0x00000700
#define MIO_CHAN_CNT_429TXL_SHFT       8
#define MIO_CHAN_CNT_232               0x00001800
#define MIO_CHAN_CNT_232_SHFT          11
#define MIO_CHAN_CNT_485               0x00006000
#define MIO_CHAN_CNT_485_SHFT          13
#define MIO_CHAN_CNT_DISC              0x000F8000
#define MIO_CHAN_CNT_DISC_SHFT         15
#define MIO_CHAN_CNT_AVION             0x01F00000
#define MIO_CHAN_CNT_AVION_SHFT        20
#define MIO_CHAN_CNT_429TXM            0x04000000
#define MIO_CHAN_CNT_429TXM_SHFT       23
#define MIO_BOARD_MODEL_TYPE           0xF0000000
#define MIO_BOARD_MODEL_SHFT           28

/* rx control register masks */
#define MIO_RX_CNT_MODE                0x00000010
#define MIO_RX_CNT_TT                  0x00000020
#define MIO_RX_CNT_SPEED               0x00000200
#define MIO_RX_CNT_FIFO_EMPTY          0x00004000
#define MIO_RX_CNT_FIFO_COUNT          0x01FF8000
#define MIO_RX_CNT_ARINC575            0x20000000
#define MIO_RX_CNT_OVERFLOW            0x40000000

/* tx control register masks */
#define MIO_TX_CNT_SPEED               0x00000010
#define MIO_TX_CNT_ARINC575            0x00000040

/* 429 specific addresses */
#define MIO_RX_FIFOMB_0                0x00200000
#define MIO_RX_FIFOMB_1                0x00200400
#define MIO_RX_FIFOMB_2                0x00200800
#define MIO_RX_FIFOMB_3                0x00200C00
#define MIO_RX_FIFOMB_4                0x00201000
#define MIO_RX_FIFOMB_5                0x00201400
#define MIO_RX_FIFOMB_6                0x00201800
#define MIO_RX_FIFOMB_7                0x00201C00
#define MIO_RX_FIFOMB_8                0x00202000
#define MIO_RX_FIFOMB_9                0x00202400
#define MIO_RX_FIFOMB_10               0x00202800
#define MIO_RX_FIFOMB_11               0x00202C00
#define MIO_RX_FIFOMB_12               0x00203000
#define MIO_RX_FIFOMB_13               0x00203400
#define MIO_RX_FIFOMB_14               0x00203800
#define MIO_RX_FIFOMB_15               0x00203C00

#define MIO_RX_FIFOMB_LEN              0x400

/* Data Match Table offsets */
#define MIO_DMT_0                      0x00206000
#define MIO_DMT_1                      0x00206020
#define MIO_DMT_2                      0x00206040
#define MIO_DMT_3                      0x00206060
#define MIO_DMT_4                      0x00206080
#define MIO_DMT_5                      0x002060A0
#define MIO_DMT_6                      0x002060C0
#define MIO_DMT_7                      0x002060E0
#define MIO_DMT_8                      0x00206100
#define MIO_DMT_9                      0x00206120
#define MIO_DMT_10                     0x00206140
#define MIO_DMT_11                     0x00206160
#define MIO_DMT_12                     0x00206180
#define MIO_DMT_13                     0x002061A0
#define MIO_DMT_14                     0x002061C0
#define MIO_DMT_15                     0x002061E0

#define MIO_DMT_LEN                    32

/* Flash OPs */
#define FLASH_OP_READ                  0x00080000     /* Read operation                */
#define FLASH_OP_WRITE                 0x00090000     /* Write operation               */
#define FLASH_OP_ERASE                 0x000A0000     /* Sector erase operation        */
#define FLASH_OP_TERM                  0xFFF7FFFF     /* Terminate operation mask      */

/* FLASH status patterns. */
#define FLASH_STATUS_DONE              0x00080000     /* Op done                       */
#define FLEXCORE_CAP_REG_INFO_MASK     0x04000000     /* flash info bit                */

#define FLASH_OP_TIMEOUT               400

/* This function reads a 32-bit word from the card's memory directly */
U32BIT DDCAPI ReadMemory32(S16BIT Card, U32BIT Address32);

/* This function writes a 32-bit word to the card's memory directly */
void DDCAPI WriteMemory32(S16BIT Card, U32BIT Address32, U32BIT Data32);

/* This function reads a 32-bit word from the card's registers directly */
U32BIT DDCAPI ReadRegister32(S16BIT Card, U32BIT Address32);

/* This function writes a 32-bit word to the card's registers directly */
void DDCAPI WriteRegister32(S16BIT Card, U32BIT Address32, U32BIT Data32);

#endif /* __HARDWARE_H__ */

