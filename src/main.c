#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/gint.h>
#include <gint/std/stdio.h>
#include <gint/std/string.h>
#include <gint/std/stdlib.h>

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

int numbytes; 
static int casioos_slip_poll(void)
{
	numbytes = slipdev_poll();
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

char printfbuffer[100];


#define UIP_LLH_LEN 0
#define UIP_BUFSIZE 1900

extern uint16_t uip_len; 
extern uint8_t uip_buf[UIP_BUFSIZE];

const char testdata[] = "\x45\x00\x00\x80\x9f\x21\x40\x00\x40\x01\x73\x45\x0a\x0a\x0a\x01" \
"\x0a\x0a\x0a\x02\x08\x00\x63\x84\x00\x07\x00\x02\xd1\x5f\xa9\x60" \
"\x00\x00\x00\x00\xa7\x24\x0f\x00\x00\x00\x00\x00\x10\x11\x12\x13" \
"\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f\x20\x21\x22\x23" \
"\x24\x25\x26\x27\x28\x29\x2a\x2b\x2c\x2d\x2e\x2f\x30\x31\x32\x33" \
"\x34\x35\x36\x37\x38\x39\x3a\x3b\x3c\x3d\x3e\x3f\x40\x41\x42\x43" \
"\x44\x45\x46\x47\x48\x49\x4a\x4b\x4c\x4d\x4e\x4f\x50\x51\x52\x53" \
"\x54\x55\x56\x57\x58\x59\x5a\x5b\x5c\x5d\x5e\x5f\x60\x61\x62\x63";

int main(void)
{
	dclear(C_WHITE);
	dtext(1, 1, C_BLACK, __TIMESTAMP__);
	dupdate();

	gint_world_switch(GINT_CALL(casioos_Serial_Init));

	gint_world_switch(GINT_CALL(casioos_slip_init));


	while (true)
	{
		/*gint_world_switch(GINT_CALL(casioos_slip_poll));
		if (numbytes != 0)
		{
			snprintf(printfbuffer, sizeof(printfbuffer), "read: %d bytes", numbytes);

			dclear(C_WHITE);
			dtext(1, 20, C_BLACK, printfbuffer);
			dupdate();
		}*/
	
		uip_len = sizeof(testdata);
		memcpy(uip_buf, testdata, sizeof(testdata));

		gint_world_switch(GINT_CALL(casioos_slip_send));
		
		getkey();
	}

	getkey();
	return 1;
}
