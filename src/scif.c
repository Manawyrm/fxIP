#include "scif.h"
#include <gint/intc.h>

#define BIT(x) (1 << (x))
#define BITMASK(end, start) (((BIT(end + 1) >> start) - 1) << start)

#define MMIO8(addr) (*(volatile uint8_t*)(addr)) 
#define MMIO16(addr) (*(volatile uint16_t*)(addr)) 

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
#define SCFSR_TDFE BIT(5) /* Doesn't seem to work on fx-9750GII? Beware! */
#define SCFSR_RDF  BIT(1)
#define SCFSR_DR   BIT(0)

#define SCFDR_TFDC_MASK BITMASK(12, 8)
#define SCFDR_TFDC_SHIFT 8
#define SCFDR_TFDC ((SCFDR0 & SCFDR_TFDC_MASK) >> SCFDR_TFDC_SHIFT)

#define SCFDR_RFDC_MASK BITMASK(4, 0)
#define SCFDR_RFDC_SHIFT 0
#define SCFDR_RFDC ((SCFDR0 & SCFDR_RFDC_MASK) >> SCFDR_RFDC_SHIFT)


#define SCIF_SCIF0_INTEVT 0xC00

#define RING_BUF_SIZE 1024
uint8_t rx_ring_buf[RING_BUF_SIZE];
uint16_t rx_ring_read  = 0;
volatile uint16_t rx_ring_write = 0; 


extern unsigned int log_idx;
uint8_t counter = 0;

void scif_debug()
{
	//A441 0000
	/*fxip_printf("SCSMR0: 0x%04x", SCSMR0);
	fxip_printf("SCBRR0: 0x%02x", SCBRR0);
	fxip_printf("SCSCR0: 0x%04x", SCSCR0);*/
	log_idx = 0;
	fxip_printf("SCFSR0: 0x%04x", SCFSR0);
	fxip_printf("SCSCR0: 0x%04x", SCSCR0);
	fxip_printf("wpos  : 0x%04x", rx_ring_write);
	fxip_printf("count : 0x%02x", counter++);

	//print_memory(cpu_getVBR() + 0x640, 0x1000);
	int data; 

	if ((data = scif_read()) > 0)
	{
		uint8_t data2 = data;
		scif_write(&data2, 1);
	}

	render_logs();

}

void scif_write(const void *data, uint16_t len)
{
	const uint8_t *data8 = data;

	while (len--)
	{
		while (SCFDR_TFDC > 8);

		SCFTDR0 = *data8++;
	}
}

int scif_read()
{
	if (rx_ring_read != rx_ring_write)
	{
		uint8_t byte = rx_ring_buf[rx_ring_read++];
		rx_ring_read %= sizeof(rx_ring_buf);
		return byte;
	}
	return -1;
}

static void scif_interrupt()
{
	// Number of Data Bytes in Receive FIFO > 0
	while (SCFDR_RFDC > 0)
	{
		rx_ring_buf[rx_ring_write++] = SCFRDR0;
		rx_ring_write %= sizeof(rx_ring_buf);
	}
	SCFSR0 &= ~SCFSR_DR;
	SCFSR0 &= ~SCFSR_RDF;
}

void scif_init()
{
	// disable RIE
	//SCSCR0 &= ~BIT(6);
	//((void(*)())0xcaffee05)();

	intc_handler_function(SCIF_SCIF0_INTEVT, scif_interrupt);
	intc_priority(INTC_SCIF0, 1);
}