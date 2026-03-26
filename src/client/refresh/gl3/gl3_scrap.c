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
 * Allocate all the little status bar objects into a single texture
 * to crutch up inefficient hardware / drivers.
 * Has the same size as a GL3 lightmap.
 *
 * =======================================================================
 */

#include "header/local.h"

static int scrap_allocated[BLOCK_WIDTH];

GLuint gl3scrap_texnum;
qboolean gl3scrap_dirty;
byte gl3scrap_texels[BLOCK_WIDTH * BLOCK_HEIGHT];

extern cvar_t *gl3_scrap;

extern qboolean GL3_Upload8(byte *data,
		int width, int height,
		qboolean mipmap, qboolean is_sky);

/* Returns if there's space inside the scrap and the position inside it */
qboolean
GL3_Scrap_AllocBlock(int w, int h, int *x, int *y)
{
	int i, j, best = BLOCK_HEIGHT, best2;

	if (gl3scrap_texnum == 0) return false;	// scrap disabled

	w += 2;	// add an empty border to all sides
	h += 2;

	for (i = 0; i < BLOCK_WIDTH - w; i++)
	{
		best2 = 0;

		for (j = 0; j < w; j++)
		{
			if (scrap_allocated[i + j] >= best)
			{
				break;
			}

			if (scrap_allocated[i + j] > best2)
			{
				best2 = scrap_allocated[i + j];
			}
		}

		if (j == w)	// this is a valid spot
		{
			*x = i;
			*y = best = best2;
		}
	}

	if (best + h > BLOCK_HEIGHT)
	{
		return false;
	}

	for (i = 0; i < w; i++)
	{
		scrap_allocated[*x + i] = best + h;
	}
	(*x)++;	// jump the border
	(*y)++;

	return true;
}


void
GL3_Scrap_Upload(void)
{
	if (!gl3scrap_dirty) return;

	GL3_SelectTMU(GL_TEXTURE0);
	GL3_Bind(gl3scrap_texnum);
	GL3_Upload8(gl3scrap_texels, BLOCK_WIDTH, BLOCK_HEIGHT, false, false);
	gl3scrap_dirty = false;
}

void
GL3_Scrap_Init(void)
{
	if (!gl3_scrap->value)	// disabled
	{
		gl3scrap_texnum = 0;
		return;
	}

	glGenTextures(1, &gl3scrap_texnum);
	memset (scrap_allocated, 0, sizeof(scrap_allocated));	// empty
	memset (gl3scrap_texels, 255, sizeof(gl3scrap_texels));	// transparent
}

