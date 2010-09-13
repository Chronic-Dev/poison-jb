/**
  * GreenPois0n Cynanide - iPhone3,1/8B117/iBSS/offsets.h
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
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

/*
 * when going thru ibss I recognized some other hw stuff that I knew the function of so I added their addresses
 * just remove if not wanted
 * ~chronic (9/11/10)
 */
//#define TARGET_TASK_CREATE                (0x16638+1)
//#define TARGET_TASK_WAIT                  (0x16574+1)
//#define TARGET_TASK_EXIT                  (0x16A7C+1)
//#define TARGET_DEVICE_POWERCTRL           (0x15968+1) // arg1=dev_num, arg2=1 for on, 0 for off. like call this(0x14, 1); to turn aes on for i4. ~chronic (9/11/10)

#ifndef OFFSETS_H
#define OFFSETS_H

// Base address
#define TARGET_BASEADDR                   (0x84000000)

// Standard offsets
#define TARGET_FREE                       (0x117D4+1)
#define TARGET_MALLOC                     (0x11A58+1)
#define TARGET_JUMP_TO                    (0x15E18+1)
#define TARGET_PRINTF                     (0x1B684+1)
#define TARGET_VPRINTF                    (0x1AF54+1)

// Command offsets
#define TARGET_CMD_LIST_BEGIN             (0x24000)
#define TARGET_CMD_LIST_END               (0x2402C)

// Task functions
#define TARGET_TASK_YIELD                 (0x16714+1)
#define TARGET_TASK_RUNNING               (0x24880)
#define TARGET_TASK_LIST                  (0x24918)

// AES offsets
#define TARGET_AES_CRYPTO_CMD             (0x17B50+1)

// BDev offsets
#define TARGET_BDEV_LIST                  (0x28440)

// Image offsets
#define TARGET_IMAGE_LIST                 (0x24850)

// Filesystem offsets
//#define TARGET_FS_MOUNT                   (0x0)
//#define TARGET_FS_UNMOUNT                 (0x0)
//#define TARGET_FS_LOAD_FILE               (0x0)

// NVRAM offsets
#define TARGET_NVRAM_LIST                 (0x24958)

#endif // OFFSETS_H
