/****************************************************************************
 * include/nuttx/streams.h
 *
 *   Copyright (C) 2009, 2011-2012, 2014 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef _INCLUDE_NUTTX_STREAMS_H
#define _INCLUDE_NUTTX_STREAMS_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* These are the generic representations of a streams used by the NuttX */

struct lib_instream_s;
typedef int  (*lib_getc_t)(FAR struct lib_instream_s *this);

struct lib_outstream_s;
typedef void (*lib_putc_t)(FAR struct lib_outstream_s *this, int ch);
typedef int  (*lib_flush_t)(FAR struct lib_outstream_s *this);

struct lib_instream_s
{
  lib_getc_t             get;     /* Get one character from the instream */
  int                    nget;    /* Total number of characters gotten.  Written
                                   * by get method, readable by user */
};

struct lib_outstream_s
{
  lib_putc_t             put;     /* Put one character to the outstream */
#ifdef CONFIG_STDIO_LINEBUFFER
  lib_flush_t            flush;   /* Flush any buffered characters in the outstream */
#endif
  int                    nput;    /* Total number of characters put.  Written
                                   * by put method, readable by user */
};

/* Seek-able streams */

struct lib_sistream_s;
typedef int   (*lib_sigetc_t)(FAR struct lib_sistream_s *this);
typedef off_t (*lib_siseek_t)(FAR struct lib_sistream_s *this, off_t offset,
                              int whence);

struct lib_sostream_s;
typedef void  (*lib_soputc_t)(FAR struct lib_sostream_s *this, int ch);
typedef int   (*lib_soflush_t)(FAR struct lib_sostream_s *this);
typedef off_t (*lib_soseek_t)(FAR struct lib_sostream_s *this, off_t offset,
                              int whence);

struct lib_sistream_s
{
  lib_sigetc_t           get;     /* Get one character from the instream */
  lib_siseek_t           seek;    /* Seek to a position in the instream */
  int                    nget;    /* Total number of characters gotten.  Written
                                   * by get method, readable by user */
};

struct lib_sostream_s
{
  lib_soputc_t           put;     /* Put one character to the outstream */
#ifdef CONFIG_STDIO_LINEBUFFER
  lib_soflush_t          flush;   /* Flush any buffered characters in the outstream */
#endif
  lib_soseek_t           seek;    /* Seek a position in the output stream */
  int                    nput;    /* Total number of characters put.  Written
                                   * by put method, readable by user */
};

/* These are streams that operate on a fixed-sized block of memory */

struct lib_meminstream_s
{
  struct lib_instream_s  public;
  FAR const char        *buffer;  /* Address of first byte in the buffer */
  size_t                 buflen;  /* Size of the buffer in bytes */
};

struct lib_memoutstream_s
{
  struct lib_outstream_s public;
  FAR char              *buffer;  /* Address of first byte in the buffer */
  size_t                 buflen;  /* Size of the buffer in bytes */
};

struct lib_memsistream_s
{
  struct lib_sistream_s  public;
  FAR const char        *buffer;  /* Address of first byte in the buffer */
  size_t                 offset;  /* Current buffer offset in bytes */
  size_t                 buflen;  /* Size of the buffer in bytes */
};

struct lib_memsostream_s
{
  struct lib_sostream_s  public;
  FAR char              *buffer;  /* Address of first byte in the buffer */
  size_t                 offset;  /* Current buffer offset in bytes */
  size_t                 buflen;  /* Size of the buffer in bytes */
};

/* These are streams that operate on a FILE */

struct lib_stdinstream_s
{
  struct lib_instream_s  public;
  FAR FILE              *stream;
};

struct lib_stdoutstream_s
{
  struct lib_outstream_s public;
  FAR FILE              *stream;
};

struct lib_stdsistream_s
{
  struct lib_sistream_s  public;
  FAR FILE              *stream;
};

struct lib_stdsostream_s
{
  struct lib_sostream_s  public;
  FAR FILE              *stream;
};

/* These are streams that operate on a file descriptor */

struct lib_rawinstream_s
{
  struct lib_instream_s  public;
  int                    fd;
};

struct lib_rawoutstream_s
{
  struct lib_outstream_s public;
  int                    fd;
};

struct lib_rawsistream_s
{
  struct lib_sistream_s  public;
  int                    fd;
};

struct lib_rawsostream_s
{
  struct lib_sostream_s  public;
  int                    fd;
};

/****************************************************************************
 * Public Variables
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#  define EXTERN extern "C"
extern "C"
{
#else
#  define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: lib_meminstream, lib_memoutstream, lib_memsistream, lib_memsostream
 *
 * Description:
 *   Initializes a stream for use with a fixed-size memory buffer.
 *   Defined in lib/stdio/lib_meminstream.c and lib/stdio/lib_memoutstream.c.
 *   Seekable versions are defined in lib/stdio/lib_memsistream.c and
 *   lib/stdio/lib_memsostream.c.
 *
 * Input parameters:
 *   memstream    - User allocated, uninitialized instance of struct
 *                  lib_meminstream_s to be initialized.
 *   memstream    - User allocated, uninitialized instance of struct
 *                  lib_memoutstream_s to be initialized.
 *   bufstart     - Address of the beginning of the fixed-size memory buffer
 *   buflen       - Size of the fixed-sized memory buffer in bytes
 *
 * Returned Value:
 *   None (User allocated instance initialized).
 *
 ****************************************************************************/

void lib_meminstream(FAR struct lib_meminstream_s *instream,
                     FAR const char *bufstart, int buflen);
void lib_memoutstream(FAR struct lib_memoutstream_s *outstream,
                      FAR char *bufstart, int buflen);
void lib_memsistream(FAR struct lib_memsistream_s *instream,
                     FAR const char *bufstart, int buflen);
void lib_memsostream(FAR struct lib_memsostream_s *outstream,
                     FAR char *bufstart, int buflen);

/****************************************************************************
 * Name: lib_stdinstream, lib_stdoutstream
 *
 * Description:
 *   Initializes a stream for use with a FILE instance.
 *   Defined in lib/stdio/lib_stdinstream.c and lib/stdio/lib_stdoutstream.c
 *
 * Input parameters:
 *   instream  - User allocated, uninitialized instance of struct
 *               lib_stdinstream_s to be initialized.
 *   outstream - User allocated, uninitialized instance of struct
 *               lib_stdoutstream_s to be initialized.
 *   stream    - User provided stream instance (must have been opened for
 *               the correct access).
 *
 * Returned Value:
 *   None (User allocated instance initialized).
 *
 ****************************************************************************/

void lib_stdinstream(FAR struct lib_stdinstream_s *instream,
                     FAR FILE *stream);
void lib_stdoutstream(FAR struct lib_stdoutstream_s *outstream,
                      FAR FILE *stream);
void lib_stdsistream(FAR struct lib_stdsistream_s *instream,
                     FAR FILE *stream);
void lib_stdsostream(FAR struct lib_stdsostream_s *outstream,
                     FAR FILE *stream);

/****************************************************************************
 * Name: lib_rawinstream, lib_rawoutstream, lib_rawsistream, and
 *       lib_rawsostream,
 *
 * Description:
 *   Initializes a stream for use with a file descriptor.
 *   Defined in lib/stdio/lib_rawinstream.c and lib/stdio/lib_rawoutstream.c.
 *   Seekable versions are defined in lib/stdio/lib_rawsistream.c and
 *   lib/stdio/lib_rawsostream.c
 *
 * Input parameters:
 *   instream  - User allocated, uninitialized instance of struct
 *               lib_rawinstream_s to be initialized.
 *   outstream - User allocated, uninitialized instance of struct
 *               lib_rawoutstream_s to be initialized.
 *   fd        - User provided file/socket descriptor (must have been opened
 *               for the correct access).
 *
 * Returned Value:
 *   None (User allocated instance initialized).
 *
 ****************************************************************************/

void lib_rawinstream(FAR struct lib_rawinstream_s *instream, int fd);
void lib_rawoutstream(FAR struct lib_rawoutstream_s *outstream, int fd);
void lib_rawsistream(FAR struct lib_rawsistream_s *instream, int fd);
void lib_rawsostream(FAR struct lib_rawsostream_s *outstream, int fd);

/****************************************************************************
 * Name: lib_lowinstream, lib_lowoutstream
 *
 * Description:
 *   Initializes a stream for use with low-level, architecture-specific I/O.
 *   Defined in lib/stdio/lib_lowinstream.c and lib/stdio/lib_lowoutstream.c
 *
 * Input parameters:
 *   lowinstream  - User allocated, uninitialized instance of struct
 *                  lib_lowinstream_s to be initialized.
 *   lowoutstream - User allocated, uninitialized instance of struct
 *                  lib_lowoutstream_s to be initialized.
 *
 * Returned Value:
 *   None (User allocated instance initialized).
 *
 ****************************************************************************/

#ifdef CONFIG_ARCH_LOWGETC
void lib_lowinstream(FAR struct lib_instream_s *lowinstream);
#endif
#ifdef CONFIG_ARCH_LOWPUTC
void lib_lowoutstream(FAR struct lib_outstream_s *lowoutstream);
#endif

/****************************************************************************
 * Name: lib_zeroinstream, lib_nullinstream, lib_nulloutstream
 *
 * Description:
 *   Initializes NULL streams:
 *
 *   o The stream created by lib_zeroinstream will return an infinitely long
 *     stream of zeroes. Defined in lib/stdio/lib_zeroinstream.c
 *   o The stream created by lib_nullinstream will return only EOF.
 *     Defined in lib/stdio/lib_nullinstream.c
 *   o The stream created by lib_nulloutstream will write all data to the
 *     bit-bucket. Defined in lib/stdio/lib_nulloutstream.c
 *
 * Input parameters:
 *   zeroinstream  - User allocated, uninitialized instance of struct
 *                   lib_instream_s to be initialized.
 *   nullinstream  - User allocated, uninitialized instance of struct
 *                   lib_instream_s to be initialized.
 *   nulloutstream - User allocated, uninitialized instance of struct
 *                   lib_outstream_s to be initialized.
 *
 * Returned Value:
 *   None (User allocated instance initialized).
 *
 ****************************************************************************/

void lib_zeroinstream(FAR struct lib_instream_s *zeroinstream);
void lib_nullinstream(FAR struct lib_instream_s *nullinstream);
void lib_nulloutstream(FAR struct lib_outstream_s *nulloutstream);

/****************************************************************************
 * Name: lib_sylogstream
 *
 * Description:
 *   Initializes a stream for use with the configured syslog interface.
 *
 * Input parameters:
 *   lowoutstream - User allocated, uninitialized instance of struct
 *                  lib_lowoutstream_s to be initialized.
 *
 * Returned Value:
 *   None (User allocated instance initialized).
 *
 ****************************************************************************/

#ifdef CONFIG_SYSLOG
void lib_syslogstream(FAR struct lib_outstream_s *stream);
#endif

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* _INCLUDE_NUTTX_STREAMS_H */
