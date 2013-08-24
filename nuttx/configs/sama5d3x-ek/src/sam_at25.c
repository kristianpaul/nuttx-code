/****************************************************************************
 * config/sama5d3x-ek/src/sam_at25.c
 *
 *   Copyright (C) 2013 Gregory Nutt. All rights reserved.
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/mount.h>

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <debug.h>

#ifdef CONFIG_SAMA5_SPI0
#  include <nuttx/spi/spi.h>
#  include <nuttx/mtd.h>
#  include <nuttx/fs/nxffs.h>

#  include "sam_spi.h"
#endif

#include "sama5d3x-ek.h"

#ifdef CONFIG_MTD_AT25

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: sam_at25_initialize
 *
 * Description:
 *   Initialize and configure the AT25 SPI Flash
 *
 ****************************************************************************/

int sam_at25_initialize(int minor)
{
  FAR struct spi_dev_s *spi;
  FAR struct mtd_dev_s *mtd;
  int ret;

  /* Get the SPI port driver */

  spi = up_spiinitialize(AT25_PORT);
  if (!spi)
    {
      fdbg("ERROR: Failed to initialize SPI port %d\n", AT25_PORT);
      return -ENODEV;
    }

  /* Now bind the SPI interface to the AT25 SPI FLASH driver */

  mtd = at25_initialize(spi);
  if (!mtd)
    {
      fdbg("ERROR: Failed to bind SPI port %d to the AT25 FLASH driver\n");
      return -ENODEV;
    }

#if defined(CONFIG_SAMA5_AT25_FTL)
  /* And finally, use the FTL layer to wrap the MTD driver as a block driver */

  ret = ftl_initialize(CONFIG_NSH_MMCSDMINOR, mtd);
  if (ret < 0)
    {
      fdbg("ERROR: Initialize the FTL layer\n");
      return ret;
    }

#elif defined(CONFIG_SAMA5_AT25_NXFFS)
  /* Initialize to provide NXFFS on the MTD interface */

  ret = nxffs_initialize(mtd);
  if (ret < 0)
    {
      fdbg("ERROR: NXFFS initialization failed: %d\n", -ret);
      return ret;
    }

  /* Mount the file system at /mnt/at25 */

  ret = mount(NULL, "/mnt/at25", "nxffs", 0, NULL);
  if (ret < 0)
    {
      fdbg("ERROR: Failed to mount the NXFFS volume: %d\n", errno);
      return ret;
    }

#endif

  return OK;
}

#endif /* HAVE_AT25_MTD */