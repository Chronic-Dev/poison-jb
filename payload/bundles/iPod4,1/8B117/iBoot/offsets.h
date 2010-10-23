/**
  * GreenPois0n Cynanide - iPod4,1/8B117/iBoot/offsets.h
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  * Copyright (C) 2010 Nicolas Haunold
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef OFFSETS_H
#define OFFSETS_H

// Base address
#define TARGET_BASEADDR                   (0x5FF00000)

// Standard offsets
#define TARGET_FREE                       (0x10DEC+1) 
#define TARGET_MALLOC                     (0x11070+1) 
#define TARGET_JUMP_TO                    (0x153EC+1)
#define TARGET_PRINTF                     (0x2AE4C+1) 
//#define TARGET_VPRINTF                    (0x2ADFC+1)

// Command offsets
#define TARGET_CMD_LIST_BEGIN             (0x37890)
#define TARGET_CMD_LIST_END               (0x378A8)
//#define TARGET_CMD_RAMDISK                (0x1058+1)

// Task functions
//#define TARGET_TASK_YIELD                 (0x15D10+1)
#define TARGET_TASK_RUNNING               (0x37734) 
#define TARGET_TASK_LIST                  (0x377CC)

// AES offsets
//#define TARGET_AES_CRYPTO_CMD             (0x17024+1)

// BDev offsets
#define TARGET_BDEV_LIST                  (0x3B940)

// Image offsets
#define TARGET_IMAGE_LIST                 (0x37704) 

// Filesystem offsets
#define TARGET_FS_MOUNT                   (0x19838+1)
#define TARGET_FS_UNMOUNT                 (0x19A50+1)
#define TARGET_FS_LOAD_FILE               (0x199E8+1)

// Kernel offsets
//#define TARGET_KERNEL_LOAD                (0x12388+1)
#define TARGET_KERNEL_PHYMEM              (0x3CA80)
//#define TARGET_KERNEL_BOOTARGS            (0x3AAC4)

// NVRAM offset
#define TARGET_NVRAM_LIST                 (0x37810) 

#endif // OFFSETS_H
