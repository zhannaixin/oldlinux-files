/* opncls.c -- open and close a BFD.
   Copyright (C) 1990-1991 Free Software Foundation, Inc.
   Written by Cygnus Support.

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id: opncls.c,v 1.42 1992/05/12 21:42:43 sac Exp $ */

#include "bfd.h"
#include "sysdep.h"
#include "libbfd.h"
#include "obstack.h"
extern void bfd_cache_init();
FILE *bfd_open_file();

/* fdopen is a loser -- we should use stdio exclusively.  Unfortunately
   if we do that we can't use fcntl.  */


#define obstack_chunk_alloc bfd_xmalloc
#define obstack_chunk_free free

/* Return a new BFD.  All BFD's are allocated through this routine.  */

bfd *new_bfd()
{
  bfd *nbfd;

  nbfd = (bfd *)zalloc (sizeof (bfd));
  if (!nbfd)
    return 0;

  bfd_check_init();
  obstack_begin((PTR)&nbfd->memory, 128);

  nbfd->arch_info = &bfd_default_arch_struct;

  nbfd->direction = no_direction;
  nbfd->iostream = NULL;
  nbfd->where = 0;
  nbfd->sections = (asection *)NULL;
  nbfd->format = bfd_unknown;
  nbfd->my_archive = (bfd *)NULL;
  nbfd->origin = 0;				   
  nbfd->opened_once = false;
  nbfd->output_has_begun = false;
  nbfd->section_count = 0;
  nbfd->usrdata = (PTR)NULL;
  nbfd->sections = (asection *)NULL;
  nbfd->cacheable = false;
  nbfd->flags = NO_FLAGS;
  nbfd->mtime_set = false;

  
  return nbfd;
}

/* Allocate a new BFD as a member of archive OBFD.  */

bfd *new_bfd_contained_in(obfd)
bfd *obfd;
{
	bfd *nbfd = new_bfd();
	nbfd->xvec = obfd->xvec;
	nbfd->my_archive = obfd;
	nbfd->direction = read_direction;
	nbfd->target_defaulted = obfd->target_defaulted;
	return nbfd;
}

/*
SECTION
	Opening and Closing BFDs

*/

/*
FUNCTION
	bfd_openr

SYNOPSIS
        bfd *bfd_openr(CONST char *filename, CONST char*target);

DESCRIPTION
	This function opens the file supplied (using <<fopen>>) with the target
	supplied, it returns a pointer to the created BFD.

	If NULL is returned then an error has occured. Possible errors
	are <<no_memory>>, <<invalid_target>> or <<system_call>> error.
*/

bfd *
DEFUN(bfd_openr, (filename, target),
      CONST char *filename AND
      CONST char *target)
{
  bfd *nbfd;
  bfd_target *target_vec;

  nbfd = new_bfd();
  if (nbfd == NULL) {
    bfd_error = no_memory;
    return NULL;
  }

  target_vec = bfd_find_target (target, nbfd);
  if (target_vec == NULL) {
    bfd_error = invalid_target;
    return NULL;
  }

  nbfd->filename = filename;
  nbfd->direction = read_direction; 

  if (bfd_open_file (nbfd) == NULL) {
    bfd_error = system_call_error;	/* File didn't exist, or some such */
    bfd_release(nbfd,0);
    return NULL;
  }
  return nbfd;
}


/* Don't try to `optimize' this function:

   o - We lock using stack space so that interrupting the locking
       won't cause a storage leak.
   o - We open the file stream last, since we don't want to have to
       close it if anything goes wrong.  Closing the stream means closing
       the file descriptor too, even though we didn't open it.
 */
/*
FUNCTION
         bfd_fdopenr

SYNOPSIS
         bfd *bfd_fdopenr(CONST char *filename, CONST char *target, int fd);

DESCRIPTION
         bfd_fdopenr is to bfd_fopenr much like  fdopen is to fopen.
	 It opens a BFD on a file already described by the @var{fd}
	 supplied. 

         Possible errors are no_memory, invalid_target and system_call
	 error.
*/

bfd *
DEFUN(bfd_fdopenr,(filename, target, fd),
      CONST char *filename AND
      CONST char *target AND
      int fd)
{
  bfd *nbfd;
  bfd_target *target_vec;
  int fdflags;

  bfd_error = system_call_error;
  
#ifdef NO_FCNTL
  fdflags = O_RDWR;			/* Assume full access */
#else
  fdflags = fcntl (fd, F_GETFL, NULL);
#endif
  if (fdflags == -1) return NULL;

  nbfd = new_bfd();

  if (nbfd == NULL) {
    bfd_error = no_memory;
    return NULL;
  }

  target_vec = bfd_find_target (target, nbfd);
  if (target_vec == NULL) {
    bfd_error = invalid_target;
    return NULL;
  }
#if defined(VMS) || defined(__GO32__)
  nbfd->iostream = (char *)fopen(filename, FOPEN_RB);
#else
#ifdef FASCIST_FDOPEN
  nbfd->iostream = (char *) fdopen (fd, FOPEN_RB); 
#else
  /* if the fd were open for read only, this still would not hurt: */
  nbfd->iostream = (char *) fdopen (fd, FOPEN_RUB); 
#endif
#endif
  if (nbfd->iostream == NULL) {
    (void) obstack_free (&nbfd->memory, (PTR)0);
    return NULL;
  }
  
  /* OK, put everything where it belongs */

  nbfd->filename = filename;

  /* As a special case we allow a FD open for read/write to
     be written through, although doing so requires that we end
     the previous clause with a preposition.  */
  /* (O_ACCMODE) parens are to avoid Ultrix header file bug */
  switch (fdflags & (O_ACCMODE)) {
  case O_RDONLY: nbfd->direction = read_direction; break;
  case O_WRONLY: nbfd->direction = write_direction; break;  
  case O_RDWR: nbfd->direction = both_direction; break;
  default: abort ();
  }
				   
  bfd_cache_init (nbfd);

  return nbfd;
}

/** bfd_openw -- open for writing.
  Returns a pointer to a freshly-allocated BFD on success, or NULL.

  See comment by bfd_fdopenr before you try to modify this function. */

/*
FUNCTION
	bfd_openw

SYNOPSIS
	bfd *bfd_openw(CONST char *filename, CONST char *target);

DESCRIPTION
	Creates a BFD, associated with file @var{filename}, using the
	file format @var{target}, and returns a pointer to it.

	Possible errors are system_call_error, no_memory,
	invalid_target. 
*/

bfd *
DEFUN(bfd_openw,(filename, target),
      CONST char *filename AND
      CONST char *target)
{
  bfd *nbfd;
  bfd_target *target_vec;
  
  bfd_error = system_call_error;

  /* nbfd has to point to head of malloc'ed block so that bfd_close may
     reclaim it correctly. */

  nbfd = new_bfd();
  if (nbfd == NULL) {
    bfd_error = no_memory;
    return NULL;
  }

  target_vec = bfd_find_target (target, nbfd);
  if (target_vec == NULL) return NULL;

  nbfd->filename = filename;
  nbfd->direction = write_direction;

  if (bfd_open_file (nbfd) == NULL) {
    bfd_error = system_call_error;	/* File not writeable, etc */
    (void) obstack_free (&nbfd->memory, (PTR)0);
    return NULL;
  }
  return nbfd;
}

/*

FUNCTION
	bfd_close

SYNOPSIS
	boolean bfd_close(bfd *);

DESCRIPTION

	This function closes a BFD. If the BFD was open for writing,
	then pending operations are completed and the file written out
	and closed. If the created file is executable, then
	<<chmod>> is called to mark it as such.

	All memory attached to the BFD's obstacks is released. 

RETURNS
	<<true>> is returned if all is ok, otherwise <<false>>.
*/


boolean
DEFUN(bfd_close,(abfd),
      bfd *abfd)
{
  if (!bfd_read_p(abfd))
    if (BFD_SEND_FMT (abfd, _bfd_write_contents, (abfd)) != true)
      return false;

  if (BFD_SEND (abfd, _close_and_cleanup, (abfd)) != true) return false;

  bfd_cache_close(abfd);

  /* If the file was open for writing and is now executable,
     make it so */
  if (abfd->direction == write_direction 
      && abfd->flags & EXEC_P) {
    struct stat buf;
    stat(abfd->filename, &buf);
#ifndef S_IXUSR
#define S_IXUSR 0100	/* Execute by owner.  */
#endif
#ifndef S_IXGRP
#define S_IXGRP 0010	/* Execute by group.  */
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001	/* Execute by others.  */
#endif

    chmod(abfd->filename, 0777  & (buf.st_mode | S_IXUSR | S_IXGRP | S_IXOTH));
  }
  (void) obstack_free (&abfd->memory, (PTR)0);
  (void) free(abfd);
  return true;
}

/*
FUNCTION
	bfd_close_all_done

SYNOPSIS
	boolean bfd_close_all_done(bfd *);

DESCRIPTION
	This function closes a BFD. It differs from <<bfd_close>>
	since it does not complete any pending operations.  This
	routine would be used if the application had just used BFD for
	swapping and didn't want to use any of the writing code.

	If the created file is executable, then <<chmod>> is called
	to mark it as such.

	All memory attached to the BFD's obstacks is released. 

RETURNS
	<<true>> is returned if all is ok, otherwise <<false>>.

*/

boolean
DEFUN(bfd_close_all_done,(abfd),
      bfd *abfd)
{
  bfd_cache_close(abfd);

  /* If the file was open for writing and is now executable,
     make it so */
  if (abfd->direction == write_direction 
      && abfd->flags & EXEC_P) {
    struct stat buf;
    stat(abfd->filename, &buf);
#ifndef S_IXUSR
#define S_IXUSR 0100	/* Execute by owner.  */
#endif
#ifndef S_IXGRP
#define S_IXGRP 0010	/* Execute by group.  */
#endif
#ifndef S_IXOTH
#define S_IXOTH 0001	/* Execute by others.  */
#endif

    chmod(abfd->filename, 0x777 &(buf.st_mode | S_IXUSR | S_IXGRP | S_IXOTH));
  }
  (void) obstack_free (&abfd->memory, (PTR)0);
  (void) free(abfd);
  return true;
}


/*
FUNCTION	
	bfd_alloc_size

SYNOPSIS
	bfd_size_type bfd_alloc_size(bfd *abfd);

DESCRIPTION
        Return the number of bytes in the obstacks connected to the
	supplied BFD.

*/

bfd_size_type
DEFUN(bfd_alloc_size,(abfd),
      bfd *abfd)
{
  struct _obstack_chunk *chunk = abfd->memory.chunk;
  size_t size = 0;
  while (chunk) {
    size += chunk->limit - &(chunk->contents[0]);
    chunk = chunk->prev;
  }
  return size;
}



/*
FUNCTION
	bfd_create

SYNOPSIS
	bfd *bfd_create(CONST char *filename, bfd *template);

DESCRIPTION
	This routine creates a new BFD in the manner of
	<<bfd_openw>>, but without opening a file. The new BFD
	takes the target from the target used by @var{template}. The
	format is always set to <<bfd_object>>. 

*/

bfd *
DEFUN(bfd_create,(filename, template),
      CONST char *filename AND
      bfd *template)
{
  bfd *nbfd = new_bfd();
  if (nbfd == (bfd *)NULL) {
    bfd_error = no_memory;
    return (bfd *)NULL;
  }
  nbfd->filename = filename;
  if(template) {
    nbfd->xvec = template->xvec;
  }
  nbfd->direction = no_direction;
  bfd_set_format(nbfd, bfd_object);
  return nbfd;
}

/* 
INTERNAL_FUNCTION
	bfd_alloc_by_size_t

SYNOPSIS
	PTR bfd_alloc_by_size_t(bfd *abfd, size_t wanted);

DESCRIPTION
	This function allocates a block of memory in the obstack
	attatched to <<abfd>> and returns a pointer to it.
*/


PTR 
DEFUN(bfd_alloc_by_size_t,(abfd, size),
      bfd *abfd AND
      size_t size)
{
  PTR res = obstack_alloc(&(abfd->memory), size);
  return res;
}

DEFUN(void bfd_alloc_grow,(abfd, ptr, size),
      bfd *abfd AND
      PTR ptr AND
      bfd_size_type size)
{
  (void)   obstack_grow(&(abfd->memory), ptr, size);
}
DEFUN(PTR bfd_alloc_finish,(abfd),
      bfd *abfd)
{
  return obstack_finish(&(abfd->memory));
}

DEFUN(PTR bfd_alloc, (abfd, size),
      bfd *abfd AND
      bfd_size_type size)
{
  return bfd_alloc_by_size_t(abfd, (size_t)size);
}

DEFUN(PTR bfd_zalloc,(abfd, size),
      bfd *abfd AND
      bfd_size_type size)
{
  PTR res = bfd_alloc(abfd, size);
  memset(res, 0, (size_t)size);
  return res;
}

DEFUN(PTR bfd_realloc,(abfd, old, size),
      bfd *abfd AND
      PTR old AND
      bfd_size_type size)
{
  PTR res = bfd_alloc(abfd, size);
  memcpy(res, old, (size_t)size);
  return res;
}










