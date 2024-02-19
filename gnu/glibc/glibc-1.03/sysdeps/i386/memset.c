/* memset -- set a block of memory to some byte value.
   For Intel 80x86, x>=3.
   Copyright (C) 1991, 1992 Free Software Foundation, Inc.
   Contributed by Torbjorn Granlund (tege@sics.se).

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with the GNU C Library; see the file COPYING.LIB.  If
not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.  */

#include <ansidecl.h>
#include <string.h>
#include <memcopy.h>

#ifdef	__GNUC__

PTR
DEFUN(memset, (dstpp, c, len),
      PTR dstpp AND int c AND size_t len)
{
  unsigned long int dstp = (unsigned long int) dstpp;

  /* This explicit register allocation
     improves code very much indeed.  */
  register op_t x asm("ax");

  x = (unsigned char) c;

  /* Clear the direction flag, so filling will move forward.  */
  asm volatile("cld");

  /* This threshold value is optimal.  */
  if (len >= 12)
    {
      /* Fill X with four copies of the char we want to fill with.  */
      x |= (x << 8);
      x |= (x << 16);

      /* There are at least some bytes to set.
	 No need to test for LEN == 0 in this alignment loop.  */

      /* Fill bytes until DSTP is aligned on a longword boundary.  */
      asm volatile("rep\n"
		   "stosb" /* %0, %2, %3 */ :
		   "=D" (dstp) :
		   "0" (dstp), "c" ((-dstp) % OPSIZ), "a" (x) :
		   "cx");
      len -= (-dstp) % OPSIZ;

      /* Fill longwords.  */
      asm volatile("rep\n"
		   "stosl" /* %0, %2, %3 */ :
		   "=D" (dstp) :
		   "0" (dstp), "c" (len / OPSIZ), "a" (x) :
		   "cx");
      len %= OPSIZ;
    }

  /* Write the last few bytes.  */
  asm volatile("rep\n"
	       "stosb" /* %0, %2, %3 */ :
	       "=D" (dstp) :
	       "0" (dstp), "c" (len), "a" (x) :
	       "cx");

  return dstpp;
}

#else
#include <sysdeps/generic/memset.c>
#endif
