#include "hayes.h"
#include "util.h"

uint8_t hayes_response_buffer[128];
uint16_t hayes_response_buffer_offset = 0;

int hayes_send(char *command)
{
	if (strlen(command) != 0)
	{
		scif_write(command, strlen(command));
		scif_write("\r\n", 2);
	}
}

int hayes_read(char *readbuf, uint16_t readbuflength, int timeout, int ignore_echo)
{
	hayes_response_buffer_offset = 0;

	uint8_t echo = ignore_echo;

	uint32_t start = clock_time();

	while (1)
	{
		int byte = scif_read();
		if (byte >= 0)
		{
			hayes_response_buffer[hayes_response_buffer_offset++] = byte;

			if (byte == '\n')
			{
				hayes_response_buffer[hayes_response_buffer_offset] = 0x00;
				if (echo == 0)
				{
					echo = 1;
				}
				else
				{
					memcpy(readbuf, hayes_response_buffer, MIN(hayes_response_buffer_offset + 1, readbuflength));
					return 1;
				}

				hayes_response_buffer_offset = 0;
			}
		}

		if (timeout)
		{
			if (start + timeout < clock_time())
			{
				fxip_log("timeout");
				return 0;
			}
		}
		
	}
}

int hayes_send_and_read(char *command, char *readbuf, uint16_t readbuflength)
{
	// flush all previous content in the read
	scif_read_flush();

	hayes_send(command);

	return hayes_read(readbuf, readbuflength, 20, 0);
}