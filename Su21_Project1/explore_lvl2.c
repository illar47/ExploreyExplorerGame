/* Copyright (c) 2012, Texas Instruments Incorporated
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

*  Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

*  Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

*  Neither the name of Texas Instruments Incorporated nor the names of
   its contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/


#include <ti/grlib/grlib.h>

static const unsigned char pixel_explore_lvl24BPP_UNCOMP[] =
{
0x3e, 0x4e, 0x4e, 0x3e, 0x3e, 
0xe5, 0xe5, 0xe5, 0xe5, 0xe5, 
0x4e, 0x00, 0x3e, 0x50, 0x15, 
0xe0, 0xff, 0x05, 0x01, 0x01, 
0x40, 0xf1, 0x04, 0x3f, 0xf5, 
0xe5, 0x00, 0xf3, 0x30, 0x03, 
0x4e, 0x3e, 0x4f, 0x10, 0x00, 
0xe5, 0xe5, 0xe5, 0x31, 0x03, 
0x34, 0x4e, 0x3e, 0x30, 0x15, 
0xe3, 0xe5, 0xe5, 0x51, 0x15
};

static const unsigned long palette_explore_lvl24BPP_UNCOMP[]=
{
	0x429581, 	0x409983, 	0x000400, 	0x000b0a, 
	0x020609, 	0x001307, 	0x000000, 	0x000000, 
	0x000000, 	0x000000, 	0x000000, 	0x000000, 
	0x000000, 	0x000000, 	0x000000, 	0xffffff
};

const tImage  explore_lvl24BPP_UNCOMP=
{
	IMAGE_FMT_4BPP_UNCOMP,
	10,
	10,
	16,
	palette_explore_lvl24BPP_UNCOMP,
	pixel_explore_lvl24BPP_UNCOMP,
};

