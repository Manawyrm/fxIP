/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: httpd-fs.c,v 1.1 2006/06/07 09:13:08 adam Exp $
 */

#include "httpd.h"
#include "httpd-fs.h"
//#include "debug.h"
#include "string.h"

#ifndef NULL
#define NULL 0
#endif /* NULL */

static u8_t httpd_fs_strcmp(const char *str1, const char *str2)
{
  u8_t i;
  i = 0;
 loop:

  if(str2[i] == 0 ||
     str1[i] == '\r' ||
     str1[i] == '\n') {
    return 0;
  }

  if(str1[i] != str2[i]) {
    return 1;
  }


  ++i;
  goto loop;
}

void to_upper(uint8_t* buffer, uint16_t length)
{
	for (int i = 0; i < length; ++i)
	{
		uint8_t c = buffer[i];

		if (c >= 'a' && c <= 'z')
		{
			buffer[i] = c - 0x20;
		}
	}
}


// maximum filesize for files to serve via HTTP (for now :D)
// Petit FatFs used in the bootloader supports only 1 open file
// the full FatFs library doesn't fit into ram, together with the uIP + NE2k driver
uint8_t data[2048];
int httpd_fs_open(const char *name, struct httpd_fs_file *file)
{
  struct httpd_fsdata_file_noconst *f;
  FRESULT fr; 

  // Original code to serve static files: 
/*
  for(f = (struct httpd_fsdata_file_noconst *)HTTPD_FS_ROOT;
      f != NULL;
      f = (struct httpd_fsdata_file_noconst *)f->next) {

    if(httpd_fs_strcmp(name, f->name) == 0)
    {
      file->data = f->data;
      file->len = f->len;
      return 1;
    }
  }*/

  to_upper(name, strlen(name));

  //myprintf("requested: %s...", name);
  fr = rom_pf_open(name);
  if (fr == FR_OK)
  {
    rom_pf_read(data, 2048, &file->len);
    file->data = data;  

    //myprintf(" found: length %ul bytes\n", file->len);
    //print_memory(data, file->len);
    
    return 1;
  }
 //myprintf(" not found\n");
  return 0;
}

void httpd_fs_init(void)
{
}
