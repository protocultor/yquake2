/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * The Wal image format
 *
 * =======================================================================
 */

#include "../ref_shared.h"

/*
 * NOTE: LoadWal() is in *_image.c because it needs the renderer-specific image_t
 */

void
GetWalInfo(const char *origname, int *width, int *height)
{
	miptex_t *mt;
	int size;
	char filename[256];

	FixFileExt(origname, "wal", filename, sizeof(filename));

	size = ri.FS_LoadFile(filename, (void **)&mt);

	if (!mt)
	{
		return;
	}

	if (size < sizeof(miptex_t))
	{
		ri.FS_FreeFile((void *)mt);
		return;
	}

	*width = LittleLong(mt->width);
	*height = LittleLong(mt->height);

	ri.FS_FreeFile((void *)mt);

	return;
}

void
GetM8Info(const char *origname, int *width, int *height)
{
	m8tex_t *mt;
	int size;
	char filename[256];

	FixFileExt(origname, "m8", filename, sizeof(filename));

	size = ri.FS_LoadFile(filename, (void **)&mt);

	if (!mt)
	{
		return;
	}

	if (size < sizeof(m8tex_t) || LittleLong (mt->version) != M8_VERSION)
	{
		ri.FS_FreeFile((void *)mt);
		return;
	}

	*width = LittleLong(mt->width[0]);
	*height = LittleLong(mt->height[0]);

	ri.FS_FreeFile((void *)mt);

	return;
}
