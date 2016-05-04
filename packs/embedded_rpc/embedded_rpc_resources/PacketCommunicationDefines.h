#ifndef PACKETCOMMUNICATIONDEFINES_H
#define PACKETCOMMUNICATIONDEFINES_H

// PacketCommunicationReport
#define IS_FIRST_REPORT_MASK 0x80
#define IS_LAST_REPORT_MASK  0x40
#define REPORT_SIZE_MASK     0x3f
#define REPORT_HEADER_SIZE(BUFFER_SIZE)  ((BUFFER_SIZE<=(REPORT_SIZE_MASK+1))?1:2)

// PacketCommunicationSLIP
#define SLIP_END             0xC0    /* indicates end of packet */
#define SLIP_BEGIN           0xC1    /* indicates end of packet */
#define SLIP_ESC             0xDA    /* indicates byte stuffing */
#define SLIP_ESC_END         0xDB    /* SLIP_ESC SLIP_ESC_END means SLIP_END data byte */
#define SLIP_ESC_BEGIN       0xDC    /* SLIP_ESC SLIP_ESC_BEGIN means SLIP_BEGIN data byte */
#define SLIP_ESC_ESC         0xDD    /* SLIP_ESC SLIP_ESC_ESC means SLIP_ESC data byte */

#endif /* PACKETCOMMUNICATIONDEFINES_H */