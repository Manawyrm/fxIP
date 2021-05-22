#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>
#include <gint/std/stdio.h>
#include <gint/std/string.h>
#include <gint/std/stdlib.h>

#include "uip/uip.h"
#include "uip/uip_arp.h"
#include "uip/timer.h"

#ifndef NULL
#define NULL (void *)0
#endif /* NULL */

int i;
uip_ipaddr_t ipaddr;
struct timer periodic_timer, arp_timer;
uint32_t ticks = 0; 



typedef char log_msg_t[32];
log_msg_t display_scroll_buf[6] = {{0}};

unsigned int log_idx = 0;

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*x))

void render_logs()
{
	dclear(C_WHITE);

	for (unsigned int i = 0; i < ARRAY_SIZE(display_scroll_buf); i++)
	{
		unsigned int line_idx = (log_idx + i) % ARRAY_SIZE(display_scroll_buf);

		dtext(1, i * 10, C_BLACK, display_scroll_buf[line_idx]);
	}
	
	dupdate();
}
void fxip_log(const char *msg)
{
	strncpy(display_scroll_buf[log_idx], msg, sizeof(log_msg_t));
	display_scroll_buf[log_idx][sizeof(log_msg_t) - 1] = 0x00;

	log_idx++;
	log_idx %= ARRAY_SIZE(display_scroll_buf);

	render_logs();
}

void uip_log(char *msg)
{
	fxip_log(msg);
}

/* 
int Serial_ReadByte                (unsigned char *dest); 0x040C
int Serial_ReadBytes               (unsigned char *dest, int max, short *size); 0x040D
int Serial_WriteByte               (unsigned char byte); 0x040E
int Serial_WriteBytes              (unsigned char *src, int size); 0x040F
int Serial_GetRxBufferSize         (void); 0x0411
int Serial_GetTxBufferFreeCapacity (void); 0x0412
int Serial_ClearReceiveBuffer      (void); 0x0413
int Serial_ClearTransmitBuffer     (void); 0x0414

0x1BB7 int Serial_Open( unsigned char *mode ); - 0x0418   
0x1BB8 int Serial_Close( int mode ); - 0x0419   
0x1BB9 int Serial_ReadOneByte( unsigned char*result ); - 0x040C   
0x1BBA int Serial_ReadNBytes( unsigned char*result, int max_size, short*actually_transferred ); - 0x040D   
0x1BBB int Serial_SpyNthByte( int byteno_to_spy, unsigned char*result ); - 0x0422   
0x1BBC int Serial_BufferedTransmitOneByte( unsigned char byte_to_transmit ); - 0x040E   
0x1BBD int Serial_DirectTransmitOneByte( unsigned char byte_to_transmit ); - 0x0410   
0x1BBE int Serial_BufferedTransmitNBytes( unsigned char*bytes_to_transmit, int requested_count ); - 0x040F   
0x1BBF int Serial_GetReceivedBytesAvailable( void ); - 0x0411   
0x1BC0 int Serial_GetFreeTransmitSpace( void ); - 0x0412   
0x1BC1 int Serial_ClearReceiveBuffer( void ); - 0x0413   
0x1BC2 int Serial_ClearTransmitBuffer( void ); - 0x0414   

In the documentation of syscall Serial_ReadBytes, dest is not a pointer, but this is unlikely. The same phenomenon occurs for the src parameter of syscall Serial_WriteBytes.
*/

int __Serial_Open                    (unsigned char *mode);
int __Serial_Close                   (int mode);
int __Serial_ReadByte                (unsigned char *dest);
int __Serial_ReadBytes               (unsigned char *dest, int max, short *size);
int __Serial_WriteByte               (unsigned char byte);
int __Serial_WriteBytes              (unsigned char *src, int size);
int __Serial_GetRxBufferSize         (void);
int __Serial_GetTxBufferFreeCapacity (void);
int __Serial_ClearReceiveBuffer      (void);
int __Serial_ClearTransmitBuffer     (void);
void __OS_InnerWait_ms ( int delay_ms );

int serial_tx_bufsize = 0;

void casioos_serial_write_and_block(unsigned char *src, int size)
{
	for (int i = 0; i < size; ++i)
	{
		while (__Serial_WriteByte(src[i]) != 0)
		{

		}

		while (__Serial_GetTxBufferFreeCapacity() != serial_tx_bufsize)
		{
			__OS_InnerWait_ms(1);
		}
	}
}

static int casioos_Serial_Init()
{
	 unsigned char settings[5]={0,9,0,0,0};
	 
	 __Serial_Open(settings);
	 
	 serial_tx_bufsize = __Serial_GetTxBufferFreeCapacity();
	 
	 return 0;
}


static void casioos_slip_init(void)
{
	slipdev_init();
}

static int casioos_slip_poll(void)
{
	uip_len = slipdev_poll();
}

static void casioos_slip_send(void)
{
	slipdev_send();
}

void slipdev_char_put(unsigned char c)
{
	while (__Serial_WriteByte(c) != 0)
	{

	}

	while (__Serial_GetTxBufferFreeCapacity() != serial_tx_bufsize)
	{
		__OS_InnerWait_ms(1);
	}
}

int slipdev_char_poll(unsigned char *c)
{
	return !__Serial_ReadByte(c);
}

int casioos_sleep(int ms)
{
	__OS_InnerWait_ms(ms);
}

char printfbuffer[100];

int main(void)
{
	fxip_log("fxIP, build date:");
	fxip_log(__TIMESTAMP__);

	/*int mockcounter = 0;

	while (true)
	{
		snprintf(printfbuffer, sizeof(printfbuffer), "hallo %d", mockcounter);
		fxip_log(printfbuffer);
		render_logs();
		mockcounter++;
		
		gint_world_switch(GINT_CALL(casioos_sleep, 1000));
	}*/

	gint_world_switch(GINT_CALL(casioos_Serial_Init));

	gint_world_switch(GINT_CALL(casioos_slip_init));


	uip_init();
	fxip_log("uip_init() done");

	uip_ipaddr(ipaddr, 10,10,10,2);
	uip_sethostaddr(ipaddr);
	uip_ipaddr(ipaddr, 10,10,10,1);
	uip_setdraddr(ipaddr);
	uip_ipaddr(ipaddr, 255,255,255,0);
	uip_setnetmask(ipaddr);

	//httpd_init();
	hello_world_init();
	fxip_log("hello_world_init() done");

	while (true)
	{
		/*
		if (numbytes != 0)
		{
			snprintf(printfbuffer, sizeof(printfbuffer), "read: %d bytes", numbytes);

			dclear(C_WHITE);
			dtext(1, 20, C_BLACK, printfbuffer);
			dupdate();
		}*/
	
		gint_world_switch(GINT_CALL(casioos_slip_poll));
		if(uip_len > 0)
		{
			fxip_log("received packet!");
			//uip_arp_ipin();
			uip_input();
			/* If the above function invocation resulted in data that
				should be sent out on the network, the global variable
				uip_len is set to a value > 0. */
			if(uip_len > 0)
			{
				//uip_arp_out();
				fxip_log("sending response");
				gint_world_switch(GINT_CALL(casioos_slip_send));
			}
			
		} 
		else if(timer_expired(&periodic_timer))
		{
			timer_reset(&periodic_timer);
			//myprintf("periodic timer fired!\n");
			for(i = 0; i < UIP_CONNS; i++)
			{
				uip_periodic(i);
				/* If the above function invocation resulted in data that
					should be sent out on the network, the global variable
					uip_len is set to a value > 0. */
				if(uip_len > 0)
				{
					//uip_arp_out();
					gint_world_switch(GINT_CALL(casioos_slip_send));
				}
			}
			
			/* Call the ARP timer function every 10 seconds. */
			if(timer_expired(&arp_timer))
			{
				timer_reset(&arp_timer);
				//myprintf("arp timer fired!\n");
				//uip_arp_timer();
			}
		}
		ticks++;
	}

	getkey();
	return 1;
}
