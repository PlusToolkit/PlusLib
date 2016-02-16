/*============================================================================
                    Copyright (c) 2004 MPB Technologies Inc.
                              All Rights Reserved

      This is unpublished proprietary source code of MPB Technologies Inc.
            No part of this software may be disclosed or published 
          without the expressed written consent of MPB Technologies Inc.
==============================================================================
    For the Linux kernel-mode library, this file defines IOCTL values
  to be used in kernel module for starting, stopping, and configuring
  the driver.  If these IOCTL values are used, the f6sconfig program
  can be used to start and stop the servoloop.
  When handling the IOCTL_F6S_CONFIG operation, use the f6s_SetConfig_IOCTL
  routine defined in f6s.h to load a configuration file.
============================================================================*/

#ifndef _F6S_IOCTL_H_
#define _F6S_IOCTL_H_

#include <linux/ioctl.h>
#include "../src/globals.h"
#include "../src/config.h"

/*
** This value is supposed to uniquely identify
** the driver.  It was unused in ioctl-number.txt.
** It can be changed, if needed, but be sure
** to recompile f6sconfig.
*/
#define F6S_IOCTL        0x81

#define IOCTL_F6S_CONFIG    _IOW(F6S_IOCTL, 0, F6sConfig)
#define IOCTL_F6S_START     _IO(F6S_IOCTL, 1)
#define IOCTL_F6S_STOP      _IO(F6S_IOCTL, 2)
#define IOCTL_F6S_ACTIVE    _IOR(F6S_IOCTL, 3, int*)

#endif // _F6S_IOCTL_H_
