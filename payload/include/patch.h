/**
  * GreenPois0n Cynanide - payload/patch.h
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

#ifndef PATCH_H
#define PATCH_H

#include "common.h"
#include "device.h"
#include "offsets.h"

const unsigned char patch_cert_seq[] = "\x1a\x78\xff\x2a\x4f\xf0\xff\x30";
const unsigned char patch_cert[] = "\x00\x20\x00\x20";

const unsigned char patch_perm_seq[] = "\xf3\xdf\x90\xb5\x07\x4b\x1b\x68";
const unsigned char patch_perm[] = "\x4f\xf0\xff\x33";

const unsigned char patch_tags_seq[] = "\x11\x9a\xd3\xf1\x18\xbf\x01\x20";
const unsigned char patch_tags[] = "\x00\x20\x00\x20";

const unsigned char patch_cmd_seq[] = "\x80\xb5\x00\xaf\x04\x28\x0b\x46";
const unsigned char patch_cmd[] = "\x00\x4b\x18\x47\x00\x00\x00\x41";

#endif /* TASK_H */
