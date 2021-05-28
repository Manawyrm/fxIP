#include "hayes.h"
#include "util.h"

uint8_t hayes_response_buffer[128];
uint16_t hayes_response_buffer_offset = 0;

int hayes_send_and_read(char *command, char* readbuf, char* readbuflength)
{
	hayes_response_buffer_offset = 0;

	scif_write(command, strlen(command));
	scif_write("\r\n", 2);

	uint8_t finished = 0;
	uint8_t echo = 0;
	while (!finished)
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
					memcpy(readbuf, hayes_response_buffer, MIN(hayes_response_buffer_offset, readbuflength));
					return 1;
				}

				hayes_response_buffer_offset = 0;
			}
		}
	}
	
	return 0;
}