/* Copyright (C) 1991 Free Software Foundation, Inc.
This file is part of the GNU C Library.

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
#include <errno.h>
#include <sys/socket.h>

/* Read N bytes into BUF through socket FD from peer
   at address ADDR (which is ADDR_LEN bytes long).
   Returns the number read or -1 for errors.  */
int
DEFUN(recvfrom, (fd, buf, n, flags, addr, addr_len),
      int fd AND PTR buf AND size_t n AND int flags AND
      struct sockaddr *addr AND size_t *addr_len)
{
  errno = ENOSYS;
  return -1;
}


#ifdef	 HAVE_GNU_LD

#include <gnu-stabs.h>

stub_warning(recvfrom);

#endif	/* GNU stabs.  */
