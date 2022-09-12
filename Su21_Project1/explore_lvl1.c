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

static const unsigned char pixel_explore_lvl14BPP_UNCOMP[] =
{
0x70, 0x00, 0x00, 0x00, 0x70, 
0x07, 0x07, 0x07, 0x07, 0x08, 
0x00, 0x70, 0x00, 0x81, 0x17, 
0x00, 0x01, 0x07, 0x11, 0x11, 
0x00, 0x11, 0x10, 0x7f, 0xf0, 
0x00, 0x01, 0xf7, 0x01, 0x18, 
0x00, 0x70, 0x71, 0x11, 0x11, 
0x00, 0x07, 0x08, 0x81, 0x18, 
0x70, 0x00, 0x00, 0x81, 0x18, 
0x07, 0x00, 0x07, 0x01, 0x18
};

static const unsigned long palette_explore_lvl14BPP_UNCOMP[]=
{
	0x000200, 	0x489585, 	0x030100, 	0x000400, 
	0x030000, 	0x050601, 	0x010200, 	0x001e0e, 
	0x002616, 	0x000000, 	0x000000, 	0x000000, 
	0x000000, 	0x000000, 	0x000000, 	0xffffff
};

const tImage  explore_lvl14BPP_UNCOMP=
{
	IMAGE_FMT_4BPP_UNCOMP,
	10,
	10,
	16,
	palette_explore_lvl14BPP_UNCOMP,
	pixel_explore_lvl14BPP_UNCOMP,
};

