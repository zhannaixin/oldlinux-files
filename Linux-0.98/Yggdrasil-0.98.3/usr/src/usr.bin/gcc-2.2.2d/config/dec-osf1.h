/* Definitions of target machine for GNU compiler.  DECstation (OSF/1) version.
   Copyright (C) 1992 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define DECSTATION
#define DEC_OSF1

#define CPP_PREDEFINES "-D__ANSI_COMPAT \
-DMIPSEL -DR3000 -DSYSTYPE_BSD -D_SYSTYPE_BSD -Dbsd4_2 -Dhost_mips -Dmips -Dosf -Dunix"

#include "decstatn.h"
