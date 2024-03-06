/*
 * Copyright (C) 2013 Alejandro Ricoveri
 * Copyright (C) 1999-2005 Id Software, Inc.
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
 * Quake GL prototypes based on ioquake3 source code
 *
 * =======================================================================
 */

#ifndef REF_QGL_H
#define REF_QGL_H

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif

#ifndef GL_EXT_shared_texture_palette
#define GL_SHARED_TEXTURE_PALETTE_EXT 0x81FB
#endif

#ifndef GL_EXT_point_parameters
#define GL_POINT_SIZE_MIN_EXT 0x8126
#define GL_POINT_SIZE_MAX_EXT 0x8127
#define GL_DISTANCE_ATTENUATION_EXT 0x8129
#endif

#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT	0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF
#endif

#ifndef GL_NV_multisample_filter_hint
#define GL_MULTISAMPLE_FILTER_HINT_NV	0x8534
#endif

#ifndef GL_ARB_imaging
#define GL_TABLE_TOO_LARGE	0x8031
#endif

#ifndef GL_VERSION_1_3
#define GL_TEXTURE0	0x84C0
#define GL_TEXTURE1	0x84C1
#define GL_TEXTURE2	0x84C2
#define GL_TEXTURE3	0x84C3
#define GL_MULTISAMPLE	0x809D
#endif

#ifndef GL_EXT_texture_env_combine
#define GL_COMBINE_EXT 0x8570
#define GL_COMBINE_RGB_EXT 0x8571
#define GL_COMBINE_ALPHA_EXT 0x8572
#define GL_RGB_SCALE_EXT 0x8573
#define GL_ADD_SIGNED_EXT 0x8574
#define GL_INTERPOLATE_EXT 0x8575
#define GL_CONSTANT_EXT 0x8576
#define GL_PRIMARY_COLOR_EXT 0x8577
#define GL_PREVIOUS_EXT 0x8578
#define GL_SOURCE0_RGB_EXT 0x8580
#define GL_SOURCE1_RGB_EXT 0x8581
#define GL_SOURCE2_RGB_EXT 0x8582
#define GL_SOURCE3_RGB_EXT 0x8583
#define GL_SOURCE4_RGB_EXT 0x8584
#define GL_SOURCE5_RGB_EXT 0x8585
#define GL_SOURCE6_RGB_EXT 0x8586
#define GL_SOURCE7_RGB_EXT 0x8587
#define GL_SOURCE0_ALPHA_EXT 0x8588
#define GL_SOURCE1_ALPHA_EXT 0x8589
#define GL_SOURCE2_ALPHA_EXT 0x858A
#define GL_SOURCE3_ALPHA_EXT 0x858B
#define GL_SOURCE4_ALPHA_EXT 0x858C
#define GL_SOURCE5_ALPHA_EXT 0x858D
#define GL_SOURCE6_ALPHA_EXT 0x858E
#define GL_SOURCE7_ALPHA_EXT 0x858F
#define GL_OPERAND0_RGB_EXT 0x8590
#define GL_OPERAND1_RGB_EXT 0x8591
#define GL_OPERAND2_RGB_EXT 0x8592
#define GL_OPERAND3_RGB_EXT 0x8593
#define GL_OPERAND4_RGB_EXT 0x8594
#define GL_OPERAND5_RGB_EXT 0x8595
#define GL_OPERAND6_RGB_EXT 0x8596
#define GL_OPERAND7_RGB_EXT 0x8597
#define GL_OPERAND0_ALPHA_EXT 0x8598
#define GL_OPERAND1_ALPHA_EXT 0x8599
#define GL_OPERAND2_ALPHA_EXT 0x859A
#define GL_OPERAND3_ALPHA_EXT 0x859B
#define GL_OPERAND4_ALPHA_EXT 0x859C
#define GL_OPERAND5_ALPHA_EXT 0x859D
#define GL_OPERAND6_ALPHA_EXT 0x859E
#define GL_OPERAND7_ALPHA_EXT 0x859F
#endif

#ifndef GL_VERSION_1_5
#ifdef _WIN64
typedef signed   long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef unsigned long  int     khronos_uintptr_t;
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
#define GL_ARRAY_BUFFER                   0x8892
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_STREAM_DRAW                    0x88E0
#define GL_STATIC_DRAW                    0x88E4
#define GL_DYNAMIC_DRAW                   0x88E8
#endif

// =======================================================================

/*
 * This is responsible for setting up our QGL extension pointers
 */
qboolean QGL_Init ( void );

/*
 * Unloads the specified DLL then nulls out all the proc pointers.
 */
void QGL_Shutdown ( void );

/* GL extensions */
extern void ( APIENTRY *qglPointParameterfARB ) ( GLenum param, GLfloat value );
extern void ( APIENTRY *qglPointParameterfvARB ) ( GLenum param,
		const GLfloat *value );
extern void ( APIENTRY *qglColorTableEXT ) ( GLenum, GLenum, GLsizei, GLenum,
		GLenum, const GLvoid * );

extern void ( APIENTRY *qglBindBuffer ) ( GLenum target, GLuint buffer );
extern void ( APIENTRY *qglDeleteBuffers ) ( GLsizei n, const GLuint *buffers );
extern void ( APIENTRY *qglGenBuffers ) ( GLsizei n, GLuint *buffers );
extern void ( APIENTRY *qglBufferData ) ( GLenum target, GLsizeiptr size,
		const void *data, GLenum usage );
extern void ( APIENTRY *qglBufferSubData ) ( GLenum target, GLintptr offset,
		GLsizeiptr size, const void *data );

#endif
