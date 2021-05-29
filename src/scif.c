#include "scif.h"
#include "util.h"
#include <gint/intc.h>
#include "ringbuffer.h"

#define SCIF_BASE 0xA4410000
#define SCIF_SIZE 0x10000

#define SCSMR(x)  MMIO16(SCIF_BASE + SCIF_SIZE * (x) + 0x00) /* Serial mode register */
#define SCBRR(x)  MMIO8 (SCIF_BASE + SCIF_SIZE * (x) + 0x04) /* Bit rate register */
#define SCSCR(x)  MMIO16(SCIF_BASE + SCIF_SIZE * (x) + 0x08) /* Serial control register */
#define SCFTDR(x) MMIO8 (SCIF_BASE + SCIF_SIZE * (x) + 0x0C) /* Transmit FIFO data register */
#define SCFSR(x)  MMIO16(SCIF_BASE + SCIF_SIZE * (x) + 0x10) /* Serial status register */
#define SCFRDR(x) MMIO8 (SCIF_BASE + SCIF_SIZE * (x) + 0x14) /* Receive FIFO data register */
#define SCFCR(x)  MMIO16(SCIF_BASE + SCIF_SIZE * (x) + 0x18) /* FIFO control register */
#define SCFDR(x)  MMIO16(SCIF_BASE + SCIF_SIZE * (x) + 0x1C) /* FIFO data count register */
#define SCLSR(x)  MMIO16(SCIF_BASE + SCIF_SIZE * (x) + 0x24) /* Line status register */

#define SCSMR0	SCSMR(0)
#define SCBRR0	SCBRR(0)
#define SCSCR0	SCSCR(0)
#define SCFTDR0	SCFTDR(0)
#define SCFSR0	SCFSR(0)
#define SCFRDR0	SCFRDR(0)
#define SCFCR0	SCFCR(0)
#define SCFDR0	SCFDR(0)
#define SCLSR0	SCLSR(0)

#define SCFSR_TEND BIT(6)
#define SCFSR_TDFE BIT(5)
#define SCFSR_RDF  BIT(1)
#define SCFSR_DR   BIT(0)

#define SCFDR_TFDC_MASK BITMASK(12, 8)
#define SCFDR_TFDC_SHIFT 8
#define SCFDR_TFDC ((SCFDR0 & SCFDR_TFDC_MASK) >> SCFDR_TFDC_SHIFT)

#define SCFDR_RFDC_MASK BITMASK(4, 0)
#define SCFDR_RFDC_SHIFT 0
#define SCFDR_RFDC ((SCFDR0 & SCFDR_RFDC_MASK) >> SCFDR_RFDC_SHIFT)

#define SCSCR_TIE BIT(7)
#define SCSCR_TE  BIT(5)

#define SCIF_SCIF0_INTEVT 0xC00

#define RING_BUF_SIZE 2048
uint8_t rx_buffer[RING_BUF_SIZE];
ringbuffer_t rx_ring;

uint8_t tx_buffer[RING_BUF_SIZE];
ringbuffer_t tx_ring;

void scif_write(const void *data, uint16_t len)
{
	while ((RING_BUF_SIZE - ringbuffer_available(&tx_ring)) < len);
	ringbuffer_write(&tx_ring, data, len);
	// enable transmit interrupt
	SCSCR0 |= SCSCR_TIE;
}

int scif_read()
{
	if (ringbuffer_available(&rx_ring))
	{
		return ringbuffer_read_one(&rx_ring);
	}
	return -1;
}

static void scif_interrupt()
{
	// fill TX FIFO
	while (SCFDR_TFDC < 16 && ringbuffer_available(&tx_ring))
	{
		SCFTDR0 = ringbuffer_read_one(&tx_ring);
	}
	if (!ringbuffer_available(&tx_ring))
	{
		// TX done, disable transmit interrupt
		SCSCR0 &= ~SCSCR_TIE;
	}
	SCFSR0 &= ~SCFSR_TDFE;

	// read RX FIFO
	// Number of Data Bytes in Receive FIFO > 0
	while (SCFDR_RFDC > 0)
	{
		ringbuffer_write_one(&rx_ring, SCFRDR0);
	}
	SCFSR0 &= ~SCFSR_DR;
	SCFSR0 &= ~SCFSR_RDF;
}

void scif_init()
{
	ringbuffer_init(&rx_ring, rx_buffer, RING_BUF_SIZE);
	ringbuffer_init(&tx_ring, tx_buffer, RING_BUF_SIZE);

	// disable transmit interrupt
	SCSCR0 &= ~SCSCR_TIE;

	intc_handler_function(SCIF_SCIF0_INTEVT, scif_interrupt);
	intc_priority(INTC_SCIF0, 1);
}