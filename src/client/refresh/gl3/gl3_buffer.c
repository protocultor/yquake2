/*
 * Copyright (C) 1997-2001 Id Software, Inc.
 * Copyright (C) 2024-2026 Jaime Moreira
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
 * Drawing buffer: sort of a "Q3A shader" handler, allows to join multiple
 * draw calls into one, by grouping those which share the same
 * characteristics (mostly the same texture).
 *
 * =======================================================================
 */

#include "header/local.h"

#define GLBUFFER_RESET	vtx_ptr = idx_ptr = 0; gl_buf.vt = 0;

glbuffer_t gl_buf;	// our drawing buffer, used globally

static GLushort vtx_ptr, idx_ptr;	// pointers for array positions in gl_buf

extern GLuint vbo2D, vao2D, ebo2D;

void
GL3_ResetGLBuffer(void)
{
	GLBUFFER_RESET
}

/*
 * Draws what's stored in the buffer and clears it up
 */
void
GL3_ApplyGLBuffer(void)
{
	// Properties of batched draws here, with default values
	GLuint shader = gl3state.si2D.shaderProgram,
		vao = vao2D,
		vbo = vbo2D,
		ebo = ebo2D;
	qboolean texture = true;

	if (vtx_ptr == 0 || idx_ptr == 0)
	{
		return;
	}

	// choosing features by type
	switch (gl_buf.type)
	{
		case buf_2d:
			shader = gl3state.si2D.shaderProgram;
			vao = vao2D;	// from gl3_draw.c
			vbo = vbo2D;
			ebo = ebo2D;
			break;
		default:
			break;
	}

	GL3_UseProgram(shader);

	if (texture)
	{
		GL3_Bind(gl_buf.texture[0]);
	}

	GL3_BindVAO(vao);
	GL3_BindVBO(vbo);
	glBufferData(GL_ARRAY_BUFFER, vtx_ptr * sizeof(GLfloat) * 4, gl_buf.data2D, GL_STREAM_DRAW);

	GL3_BindEBO(ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_ptr * sizeof(GLushort), gl_buf.idx, GL_STREAM_DRAW);

	// All set, we can finally draw
	glDrawElements(GL_TRIANGLES, idx_ptr, GL_UNSIGNED_SHORT, NULL);

	GLBUFFER_RESET
}

/*
 * Sets current state of the buffer. Any change means "draw its contents now".
 */
void
GL3_UpdateGLBuffer(buffered_draw_t type, int colortex, int lighttex,
		int flags, float alpha)
{
	if ( gl_buf.type != type || gl_buf.texture[0] != colortex )
	{
		GL3_ApplyGLBuffer();

		gl_buf.type = type;
		gl_buf.texture[0] = colortex;
		gl_buf.texture[1] = lighttex;
		gl_buf.flags = flags;
		gl_buf.alpha = alpha;
	}
}

/*
 * Stores a 2D drawing in the buffer.
  */
void
GL3_Buffer2DQuad(float x, float y, float w, float h,
			float sl, float tl, float sh, float th)
{
	static const GLushort idx_max = MAX_INDICES - 7;
	static const GLushort vtx_max = MAX_VERTICES - 5;

	if (idx_ptr > idx_max || vtx_ptr > vtx_max)
	{
		GL3_ApplyGLBuffer();
	}

	// "Quad" = 2-triangle GL_TRIANGLE_FAN
	gl_buf.idx[idx_ptr]   = vtx_ptr;
	gl_buf.idx[idx_ptr+1] = vtx_ptr+1;
	gl_buf.idx[idx_ptr+2] = vtx_ptr+2;
	gl_buf.idx[idx_ptr+3] = vtx_ptr;
	gl_buf.idx[idx_ptr+4] = vtx_ptr+2;
	gl_buf.idx[idx_ptr+5] = vtx_ptr+3;
	idx_ptr += 6;

	// Data
	gl_buf.data2D[gl_buf.vt]    = x;
	gl_buf.data2D[gl_buf.vt+ 1] = y + h;
	gl_buf.data2D[gl_buf.vt+ 2] = sl;
	gl_buf.data2D[gl_buf.vt+ 3] = th;
	gl_buf.data2D[gl_buf.vt+ 4] = x;
	gl_buf.data2D[gl_buf.vt+ 5] = y;
	gl_buf.data2D[gl_buf.vt+ 6] = sl;
	gl_buf.data2D[gl_buf.vt+ 7] = tl;
	gl_buf.data2D[gl_buf.vt+ 8] = x + w;
	gl_buf.data2D[gl_buf.vt+ 9] = y + h;
	gl_buf.data2D[gl_buf.vt+10] = sh;
	gl_buf.data2D[gl_buf.vt+11] = th;
	gl_buf.data2D[gl_buf.vt+12] = x + w;
	gl_buf.data2D[gl_buf.vt+13] = y;
	gl_buf.data2D[gl_buf.vt+14] = sh;
	gl_buf.data2D[gl_buf.vt+15] = tl;

	vtx_ptr += 4;
	gl_buf.vt += 16;
}
