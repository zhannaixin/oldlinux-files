/* Copyright (C) 1991, 1992 Free Software Foundation, Inc.
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

#define	ENTRY(name)							      \
  .text;								      \
  .globl _##name;							      \
  .align 4;								      \
  _##name##:

#define	SYSCALL_TRAP(name, number)					      \
  ENTRY (name)								      \
  lea number, %eax;							      \
  /* lcall $7, $0; */							      \
  /* Above loses; GAS bug.  */						      \
  .byte 0x9a, 0, 0, 0, 0, 7, 0;						      \
  _##name##_syscall_pc:							      \
  ret

#define MOVE(x,y)	movl x , y
