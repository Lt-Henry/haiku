/*
** Copyright 2001 Brian J. Swetland
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions, and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions, and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. The name of the author may not be used to endorse or promote products
**    derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
** IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
** INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
** NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
** THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>

struct FramebufferFont {
	int glyphWidth;
	int glyphHeight;
	uint8_t data[1548];
};

FramebufferFont smallFont = { 6, 12, {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ' ' */
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ' ' */
0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, /* '!' */
0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* '"' */
0x00, 0x00, 0x14, 0x14, 0x3e, 0x14, 0x3e, 0x14, 0x14, 0x00, 0x00, 0x00, /* '#' */
0x00, 0x00, 0x08, 0x3c, 0x0a, 0x1c, 0x28, 0x1e, 0x08, 0x00, 0x00, 0x00, /* '$' */
0x00, 0x00, 0x06, 0x26, 0x10, 0x08, 0x04, 0x32, 0x30, 0x00, 0x00, 0x00, /* '%' */
0x00, 0x00, 0x1c, 0x02, 0x02, 0x04, 0x2a, 0x12, 0x2c, 0x00, 0x00, 0x00, /* '&' */
0x00, 0x18, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* ''' */
0x20, 0x10, 0x10, 0x08, 0x08, 0x08, 0x08, 0x08, 0x10, 0x10, 0x20, 0x00, /* '(' */
0x02, 0x04, 0x04, 0x08, 0x08, 0x08, 0x08, 0x08, 0x04, 0x04, 0x02, 0x00, /* ')' */
0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, /* '*' */
0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, /* '+' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, /* ',' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* '-' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, /* '.' */
0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x04, 0x04, 0x02, 0x02, 0x00, 0x00, /* '/' */
0x00, 0x1c, 0x22, 0x32, 0x2a, 0x26, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* '0' */
0x00, 0x08, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /* '1' */
0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3e, 0x00, 0x00, 0x00, /* '2' */
0x00, 0x1c, 0x22, 0x20, 0x18, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /* '3' */
0x00, 0x20, 0x30, 0x28, 0x24, 0x22, 0x3e, 0x20, 0x20, 0x00, 0x00, 0x00, /* '4' */
0x00, 0x3e, 0x02, 0x02, 0x1e, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /* '5' */
0x00, 0x18, 0x04, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* '6' */
0x00, 0x3e, 0x22, 0x20, 0x20, 0x10, 0x10, 0x08, 0x08, 0x00, 0x00, 0x00, /* '7' */
0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* '8' */
0x00, 0x1c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x10, 0x0c, 0x00, 0x00, 0x00, /* '9' */
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, /* ':' */
0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x08, 0x04, 0x00, /* ';' */
0x00, 0x00, 0x00, 0x30, 0x0c, 0x03, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, /* '<' */
0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, /* '=' */
0x00, 0x00, 0x00, 0x03, 0x0c, 0x30, 0x0c, 0x03, 0x00, 0x00, 0x00, 0x00, /* '>' */
0x00, 0x1c, 0x22, 0x20, 0x10, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, /* '?' */
0x00, 0x00, 0x1c, 0x22, 0x3a, 0x3a, 0x1a, 0x02, 0x1c, 0x00, 0x00, 0x00, /* '@' */
0x00, 0x00, 0x08, 0x14, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00, 0x00, /* 'A' */
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, /* 'B' */
0x00, 0x00, 0x1c, 0x22, 0x02, 0x02, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, /* 'C' */
0x00, 0x00, 0x0e, 0x12, 0x22, 0x22, 0x22, 0x12, 0x0e, 0x00, 0x00, 0x00, /* 'D' */
0x00, 0x00, 0x3e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, /* 'E' */
0x00, 0x00, 0x3e, 0x02, 0x02, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /* 'F' */
0x00, 0x00, 0x1c, 0x22, 0x02, 0x32, 0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, /* 'G' */
0x00, 0x00, 0x22, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /* 'H' */
0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x3e, 0x00, 0x00, 0x00, /* 'I' */
0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* 'J' */
0x00, 0x00, 0x22, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x22, 0x00, 0x00, 0x00, /* 'K' */
0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3e, 0x00, 0x00, 0x00, /* 'L' */
0x00, 0x00, 0x22, 0x36, 0x2a, 0x2a, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /* 'M' */
0x00, 0x00, 0x22, 0x26, 0x26, 0x2a, 0x32, 0x32, 0x22, 0x00, 0x00, 0x00, /* 'N' */
0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* 'O' */
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /* 'P' */
0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x30, 0x00, 0x00, /* 'Q' */
0x00, 0x00, 0x1e, 0x22, 0x22, 0x1e, 0x0a, 0x12, 0x22, 0x00, 0x00, 0x00, /* 'R' */
0x00, 0x00, 0x1c, 0x22, 0x02, 0x1c, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, /* 'S' */
0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /* 'T' */
0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* 'U' */
0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, /* 'V' */
0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x36, 0x22, 0x00, 0x00, 0x00, /* 'W' */
0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00, 0x00, /* 'X' */
0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, /* 'Y' */
0x00, 0x00, 0x3e, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3e, 0x00, 0x00, 0x00, /* 'Z' */
0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00, /* '[' */
0x02, 0x02, 0x04, 0x04, 0x08, 0x08, 0x10, 0x10, 0x20, 0x20, 0x00, 0x00, /* '\' */
0x0e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0e, 0x00, /* ']' */
0x00, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* '^' */
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, /* '_' */
0x00, 0x0c, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* '`' */
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x32, 0x2c, 0x00, 0x00, 0x00, /* 'a' */
0x00, 0x02, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, /* 'b' */
0x00, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x02, 0x02, 0x3c, 0x00, 0x00, 0x00, /* 'c' */
0x00, 0x20, 0x20, 0x20, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x00, 0x00, 0x00, /* 'd' */
0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x3e, 0x02, 0x1c, 0x00, 0x00, 0x00, /* 'e' */
0x00, 0x38, 0x04, 0x04, 0x1e, 0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, /* 'f' */
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x1c, /* 'g' */
0x00, 0x02, 0x02, 0x02, 0x1e, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /* 'h' */
0x00, 0x08, 0x08, 0x00, 0x0c, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, /* 'i' */
0x00, 0x10, 0x10, 0x00, 0x1c, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0e, /* 'j' */
0x00, 0x02, 0x02, 0x02, 0x12, 0x0a, 0x06, 0x0a, 0x12, 0x00, 0x00, 0x00, /* 'k' */
0x00, 0x0c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, /* 'l' */
0x00, 0x00, 0x00, 0x00, 0x16, 0x2a, 0x2a, 0x2a, 0x22, 0x00, 0x00, 0x00, /* 'm' */
0x00, 0x00, 0x00, 0x00, 0x1a, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, /* 'n' */
0x00, 0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, /* 'o' */
0x00, 0x00, 0x00, 0x00, 0x1e, 0x22, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x02, /* 'p' */
0x00, 0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x20, /* 'q' */
0x00, 0x00, 0x00, 0x00, 0x1a, 0x06, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, /* 'r' */
0x00, 0x00, 0x00, 0x00, 0x3c, 0x02, 0x1c, 0x20, 0x1e, 0x00, 0x00, 0x00, /* 's' */
0x00, 0x08, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x08, 0x30, 0x00, 0x00, 0x00, /* 't' */
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x32, 0x2c, 0x00, 0x00, 0x00, /* 'u' */
0x00, 0x00, 0x00, 0x00, 0x36, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, /* 'v' */
0x00, 0x00, 0x00, 0x00, 0x22, 0x2a, 0x2a, 0x2a, 0x14, 0x00, 0x00, 0x00, /* 'w' */
0x00, 0x00, 0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, /* 'x' */
0x00, 0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x1c, /* 'y' */
0x00, 0x00, 0x00, 0x00, 0x3e, 0x10, 0x08, 0x04, 0x3e, 0x00, 0x00, 0x00, /* 'z' */
0x20, 0x10, 0x10, 0x10, 0x10, 0x08, 0x10, 0x10, 0x10, 0x10, 0x20, 0x00, /* '{' */
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, /* '|' */
0x02, 0x04, 0x04, 0x04, 0x04, 0x08, 0x04, 0x04, 0x04, 0x04, 0x02, 0x00, /* '}' */
0x00, 0x04, 0x2a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* '~' */
0x00, 0x00, 0x00, 0x08, 0x08, 0x14, 0x14, 0x22, 0x3e, 0x00, 0x00, 0x00, /* '' */
}};