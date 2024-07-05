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
 * Surface generation and drawing
 *
 * =======================================================================
 */

#include <assert.h>

#include "header/local.h"

#define DYNAMIC_COPIES (MAX_LIGHTMAP_COPIES - 1)	// last one is the static lightmap

typedef struct
{
	int top, bottom, left, right;
} lmrect_t;

int c_visible_lightmaps;
int c_visible_textures;
static vec3_t modelorg; /* relative to viewpoint */
msurface_t *r_alpha_surfaces;

gllightmapstate_t gl_lms;
glbuffer_t gl_buf;

static qboolean dynamic_frame[MAX_LIGHTMAPS];
static int cur_lm_copy;

void LM_InitBlock(void);
void LM_UploadBlock(qboolean dynamic);
qboolean LM_AllocBlock(int w, int h, int *x, int *y);

void R_SetCacheState(msurface_t *surf);
void R_BuildLightMap(msurface_t *surf, byte *dest, int stride);

extern void R_MYgluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

void
R_ApplyGLBuffer(void)
{
	// add properties of buffered draws here
	GLint vtx_size;
	qboolean mtex, alias, texture, color, alpha, texenv_set;
	float fovy, dist, intens;

	if (gl_buf.vtx_ptr == 0 || gl_buf.idx_ptr == 0)
	{
		return;
	}

	// defaults for drawing (mostly buf_singletex features)
	vtx_size = 3;
	mtex = alias = color = alpha = texenv_set = false;
	texture = true;

	// choosing features by type
	switch (gl_buf.type)
	{
		case buf_2d:
			vtx_size = 2;
			break;
		case buf_mtex:
			mtex = true;
			break;
		case buf_alias:
			alias = color = true;
			break;
		case buf_flash:
			texture = false;
			color = true;
			break;
		case buf_alpha:
			alpha = true;
			break;
		default:
			break;
	}

	R_EnableMultitexture(mtex);

	if (alias)
	{
		if (gl_buf.flags & RF_DEPTHHACK)
		{
			// hack the depth range to prevent view model from poking into walls
			glDepthRange(gldepthmin, gldepthmin + 0.3 * (gldepthmax - gldepthmin));
		}

		if (gl_buf.flags & RF_WEAPONMODEL)
		{
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();

			if (gl_lefthand->value == 1.0f)
			{
				glScalef(-1, 1, 1);
			}

			fovy = (r_gunfov->value < 0) ? r_newrefdef.fov_y : r_gunfov->value;
			dist = (r_farsee->value == 0) ? 4096.0f : 8192.0f;
			R_MYgluPerspective(fovy, (float)r_newrefdef.width / r_newrefdef.height, 4, dist);

			glMatrixMode(GL_MODELVIEW);

			if (gl_lefthand->value == 1.0f)
			{
				glCullFace(GL_BACK);
			}
		}

		glShadeModel(GL_SMOOTH);
		R_TexEnv(GL_MODULATE);

		if (gl_buf.flags & RF_TRANSLUCENT)
		{
			glEnable(GL_BLEND);
		}
	}

	if (alpha)
	{
		// the textures are prescaled up for a better
		// lighting range, so scale it back down
		intens = gl_state.inverse_intensity;
		glColor4f(intens, intens, intens, gl_buf.currentalpha);
	}
	else if (gl_buf.flags & SURF_DRAWTURB)
	{
		texenv_set = true;
		if (gl1_overbrightbits->value)
		{
			R_TexEnv(GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
		}
		else
		{
			R_TexEnv(GL_MODULATE);
			glColor4f(gl_state.inverse_intensity, gl_state.inverse_intensity,
					  gl_state.inverse_intensity, 1.0f);
		}
	}

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer (vtx_size, GL_FLOAT, 0, gl_buf.vtx);

	if (texture)
	{
		if (mtex)
		{
			// TMU 1: Lightmap texture
			// We check here if it's static or dynamic
			const int ct = gl_buf.currenttexture[1];
			int lmtexture = gl_state.lightmap_textures + ct;
			if (gl_config.lightmapcopies && dynamic_frame[ct])
			{
				// multicopy enabled: bind appropiate dynamic lightmap
				lmtexture += gl_state.max_lightmaps * (cur_lm_copy + 1);
			}
			R_MBind(GL_TEXTURE1, lmtexture);

			if (gl1_overbrightbits->value)
			{
				R_TexEnv(GL_COMBINE);
				glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, gl1_overbrightbits->value);
			}

			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, 0, gl_buf.tex[1]);

			// TMU 0: Color texture
			R_MBind(GL_TEXTURE0, gl_buf.currenttexture[0]);
		}
		else
		{
			R_Bind(gl_buf.currenttexture[0]);
		}

		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, gl_buf.tex[0]);
	}

	if (color)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, gl_buf.clr);
	}

	glDrawElements(GL_TRIANGLES, gl_buf.idx_ptr, GL_UNSIGNED_SHORT, gl_buf.idx);

	if (color)
	{
		glDisableClientState(GL_COLOR_ARRAY);
	}

	if (texture)
	{
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}

	glDisableClientState( GL_VERTEX_ARRAY );

	if (texenv_set)
	{
		R_TexEnv(GL_REPLACE);
	}

	if (alias)	// turn back everything
	{
		if (gl_buf.flags & RF_TRANSLUCENT)
		{
			glDisable(GL_BLEND);
		}

		R_TexEnv(GL_REPLACE);
		glShadeModel(GL_FLAT);

		if (gl_buf.flags & RF_WEAPONMODEL)
		{
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			if (gl_lefthand->value == 1.0F)
			{
				glCullFace(GL_FRONT);
			}
		}

		if (gl_buf.flags & RF_DEPTHHACK)
		{
			glDepthRange(gldepthmin, gldepthmax);
		}
	}

	gl_buf.vtx_ptr = gl_buf.idx_ptr = 0;
}

void
R_UpdateGLBuffer(buffered_draw_t type, int colortex, int lighttex, int flags, float alpha)
{
	if (gl_buf.type != type || gl_buf.currenttexture[0] != colortex)
	{
		goto apply_buffer;
	}

	if (gl_config.multitexture && type == buf_mtex && gl_buf.currenttexture[1] != lighttex)
	{
		goto apply_buffer;
	}

	if ((type == buf_singletex || type == buf_alias) && gl_buf.flags != flags)
	{
		goto apply_buffer;
	}

	if (type == buf_alpha && gl_buf.currentalpha != alpha)
	{
		goto apply_buffer;
	}

	return;

apply_buffer:
	R_ApplyGLBuffer();

	gl_buf.type = type;
	gl_buf.currenttexture[0] = colortex;
	gl_buf.currenttexture[1] = lighttex;
	gl_buf.flags = flags;
	gl_buf.currentalpha = alpha;
	/*
	if ( gl_config.multitexture && type == buf_mtex )	// ???
	{
		gl_buf.currenttexture[1] = lighttex;
	}
	else if (type == buf_alias)
	{
		gl_buf.flags = flags;
	}
	else if (type == buf_alpha)
	{
		gl_buf.currentalpha = alpha;
	}
	*/
}

static void
R_DrawGLPoly(glpoly_t *p)
{
	float *v;
	int	nv, i, j, k;

	nv = p->numverts;

	j = gl_buf.vtx_ptr * 3;      // vertex index
	k = gl_buf.vtx_ptr * 2;      // texcoord index

	for (i=0; i < nv-2; i++) {
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr;
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr+i+1;
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr+i+2;
	}

	for (i = 0, v = p->verts[0]; i < nv; i++, v += VERTEXSIZE)
	{
		gl_buf.vtx[j] = v[ 0 ];
		gl_buf.vtx[j+1] = v[ 1 ];
		gl_buf.vtx[j+2] = v[ 2 ];
		j += 3;
		gl_buf.tex[0][k] = v[ 3 ];
		gl_buf.tex[0][k+1] = v[ 4 ];
		k += 2;

	}

	gl_buf.vtx_ptr += nv;

	/*
	v = p->verts[0];

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 3, GL_FLOAT, VERTEXSIZE*sizeof(GLfloat), v );
    glTexCoordPointer( 2, GL_FLOAT, VERTEXSIZE*sizeof(GLfloat), v+3 );
    glDrawArrays( GL_TRIANGLE_FAN, 0, p->numverts );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	*/
}

static void
R_DrawGLFlowingPoly(msurface_t *fa)
{
	int nv, i, j, k;
	float *v;
	glpoly_t *p;
	float scroll;

	p = fa->polys;
	nv = p->numverts;

	j = gl_buf.vtx_ptr * 3;      // vertex index
	k = gl_buf.vtx_ptr * 2;      // texcoord index

	scroll = -64 * ((r_newrefdef.time / 40.0) - (int)(r_newrefdef.time / 40.0));

	if (scroll == 0.0)
	{
		scroll = -64.0;
	}

	for (i=0; i < nv-2; i++) {
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr;
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr+i+1;
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr+i+2;
	}

	for (i = 0, v = p->verts[0]; i < nv; i++, v += VERTEXSIZE)
	{
		gl_buf.vtx[j] = v[ 0 ];
		gl_buf.vtx[j+1] = v[ 1 ];
		gl_buf.vtx[j+2] = v[ 2 ];
		j += 3;
		gl_buf.tex[0][k] = v[ 3 ] + scroll;
		gl_buf.tex[0][k+1] = v[ 4 ];
		k += 2;
	}

	gl_buf.vtx_ptr += nv;

	/*
    YQ2_VLA(GLfloat, tex, 2*p->numverts);
    unsigned int index_tex = 0;

    v = p->verts [ 0 ];

	for ( i = 0; i < p->numverts; i++, v += VERTEXSIZE )
    {
        tex[index_tex++] = v [ 3 ] + scroll;
        tex[index_tex++] = v [ 4 ];
    }
    v = p->verts [ 0 ];

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );

    glVertexPointer( 3, GL_FLOAT, VERTEXSIZE*sizeof(GLfloat), v );
    glTexCoordPointer( 2, GL_FLOAT, 0, tex );
    glDrawArrays( GL_TRIANGLE_FAN, 0, p->numverts );

    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

	YQ2_VLAFREE(tex);
	*/
}

static void
R_DrawTriangleOutlines(void)
{
	int i, j;
	glpoly_t *p;

	if (!gl_showtris->value)
	{
		return;
	}

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glColor4f(1, 1, 1, 1);

	for (i = 0; i < gl_state.max_lightmaps; i++)
	{
		msurface_t *surf;

		for (surf = gl_lms.lightmap_surfaces[i];
			 surf != 0;
			 surf = surf->lightmapchain)
		{
			p = surf->polys;

			for ( ; p; p = p->chain)
			{
				for (j = 2; j < p->numverts; j++)
				{
                    GLfloat vtx[12];
                    unsigned int k;

                    for (k=0; k<3; k++)
                    {
                       vtx[0+k] = p->verts [ 0 ][ k ];
                        vtx[3+k] = p->verts [ j - 1 ][ k ];
                        vtx[6+k] = p->verts [ j ][ k ];
                        vtx[9+k] = p->verts [ 0 ][ k ];
                    }

                    glEnableClientState( GL_VERTEX_ARRAY );

                    glVertexPointer( 3, GL_FLOAT, 0, vtx );
                    glDrawArrays( GL_LINE_STRIP, 0, 4 );

                    glDisableClientState( GL_VERTEX_ARRAY );
				}
			}
		}
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}

static void
R_DrawGLPolyChain(glpoly_t *p, float soffset, float toffset)
{
	if ((soffset == 0) && (toffset == 0))
	{
		for ( ; p != 0; p = p->chain)
		{
			float *v;

			v = p->verts[0];

            glEnableClientState( GL_VERTEX_ARRAY );
            glEnableClientState( GL_TEXTURE_COORD_ARRAY );

            glVertexPointer( 3, GL_FLOAT, VERTEXSIZE*sizeof(GLfloat), v );
            glTexCoordPointer( 2, GL_FLOAT, VERTEXSIZE*sizeof(GLfloat), v+5 );
            glDrawArrays( GL_TRIANGLE_FAN, 0, p->numverts );

            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		}
	}
	else
	{
		// workaround for lack of VLAs (=> our workaround uses alloca() which is bad in loops)
#ifdef _MSC_VER
		int maxNumVerts = 0;
		for (glpoly_t* tmp = p; tmp; tmp = tmp->chain)
		{
			if ( tmp->numverts > maxNumVerts )
				maxNumVerts = tmp->numverts;
		}

		YQ2_VLA( GLfloat, tex, 2 * maxNumVerts );
#endif

		for ( ; p != 0; p = p->chain)
		{
			float *v;
			int j;

			v = p->verts[0];
#ifndef _MSC_VER // we have real VLAs, so it's safe to use one in this loop
            YQ2_VLA(GLfloat, tex, 2*p->numverts);
#endif

            unsigned int index_tex = 0;

			for ( j = 0; j < p->numverts; j++, v += VERTEXSIZE )
			{
			    tex[index_tex++] = v [ 5 ] - soffset;
			    tex[index_tex++] = v [ 6 ] - toffset;
			}

			v = p->verts [ 0 ];

            glEnableClientState( GL_VERTEX_ARRAY );
            glEnableClientState( GL_TEXTURE_COORD_ARRAY );

            glVertexPointer( 3, GL_FLOAT, VERTEXSIZE*sizeof(GLfloat), v );
            glTexCoordPointer( 2, GL_FLOAT, 0, tex );
            glDrawArrays( GL_TRIANGLE_FAN, 0, p->numverts );

            glDisableClientState( GL_VERTEX_ARRAY );
            glDisableClientState( GL_TEXTURE_COORD_ARRAY );
		}

		YQ2_VLAFREE( tex );
	}
}

/*
 * This routine takes all the given light mapped surfaces
 * in the world and blends them into the framebuffer.
 */
static void
R_BlendLightmaps(const model_t *currentmodel)
{
	int i;
	msurface_t *surf, *newdrawsurf = 0;

	/* don't bother if we're set to fullbright or multitexture is enabled */
	if (gl_config.multitexture || r_fullbright->value || !r_worldmodel->lightdata)
	{
		return;
	}

	/* don't bother writing Z */
	glDepthMask(GL_FALSE);

	/* set the appropriate blending mode unless
	   we're only looking at the lightmaps. */
	if (!gl_lightmap->value)
	{
		glEnable(GL_BLEND);

		if (gl1_saturatelighting->value)
		{
			glBlendFunc(GL_ONE, GL_ONE);
		}
		else
		{
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);
		}
	}

	if (currentmodel == r_worldmodel)
	{
		c_visible_lightmaps = 0;
	}

	/* render static lightmaps first */
	for (i = 1; i < gl_state.max_lightmaps; i++)
	{
		if (gl_lms.lightmap_surfaces[i])
		{
			if (currentmodel == r_worldmodel)
			{
				c_visible_lightmaps++;
			}

			R_Bind(gl_state.lightmap_textures + i);

			for (surf = gl_lms.lightmap_surfaces[i];
				 surf != 0;
				 surf = surf->lightmapchain)
			{
				if (surf->polys)
				{
					// Apply overbright bits to the static lightmaps
					if (gl1_overbrightbits->value)
					{
						R_TexEnv(GL_COMBINE);
						glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, gl1_overbrightbits->value);
					}

					R_DrawGLPolyChain(surf->polys, 0, 0);
				}
			}
		}
	}

	/* render dynamic lightmaps */
	if (gl1_dynamic->value)
	{
		LM_InitBlock();

		R_Bind(gl_state.lightmap_textures + 0);

		if (currentmodel == r_worldmodel)
		{
			c_visible_lightmaps++;
		}

		newdrawsurf = gl_lms.lightmap_surfaces[0];

		for (surf = gl_lms.lightmap_surfaces[0];
			 surf != 0;
			 surf = surf->lightmapchain)
		{
			int smax, tmax;
			byte *base;

			smax = (surf->extents[0] >> 4) + 1;
			tmax = (surf->extents[1] >> 4) + 1;

			if (LM_AllocBlock(smax, tmax, &surf->dlight_s, &surf->dlight_t))
			{
				base = gl_lms.lightmap_buffer[0][0];
				base += (surf->dlight_t * gl_state.block_width +
						surf->dlight_s) * LIGHTMAP_BYTES;

				R_BuildLightMap(surf, base, gl_state.block_width * LIGHTMAP_BYTES);
			}
			else
			{
				msurface_t *drawsurf;

				/* upload what we have so far */
				LM_UploadBlock(true);

				/* draw all surfaces that use this lightmap */
				for (drawsurf = newdrawsurf;
					 drawsurf != surf;
					 drawsurf = drawsurf->lightmapchain)
				{
					if (drawsurf->polys)
					{
						// Apply overbright bits to the dynamic lightmaps
						if (gl1_overbrightbits->value)
						{
							R_TexEnv(GL_COMBINE);
							glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, gl1_overbrightbits->value);
						}

						R_DrawGLPolyChain(drawsurf->polys,
								(drawsurf->light_s - drawsurf->dlight_s) * (1.0 / (float)gl_state.block_width),
								(drawsurf->light_t - drawsurf->dlight_t) * (1.0 / (float)gl_state.block_height));
					}
				}

				newdrawsurf = drawsurf;

				/* clear the block */
				LM_InitBlock();

				/* try uploading the block now */
				if (!LM_AllocBlock(smax, tmax, &surf->dlight_s, &surf->dlight_t))
				{
					ri.Sys_Error(ERR_FATAL,
							"Consecutive calls to LM_AllocBlock(%d,%d) failed (dynamic)\n",
							smax, tmax);
				}

				base = gl_lms.lightmap_buffer[0][0];
				base += (surf->dlight_t * gl_state.block_width +
						surf->dlight_s) * LIGHTMAP_BYTES;

				R_BuildLightMap(surf, base, gl_state.block_width * LIGHTMAP_BYTES);
			}
		}

		/* draw remainder of dynamic lightmaps that haven't been uploaded yet */
		if (newdrawsurf)
		{
			LM_UploadBlock(true);
		}

		for (surf = newdrawsurf; surf != 0; surf = surf->lightmapchain)
		{
			if (surf->polys)
			{
				// Apply overbright bits to the remainder lightmaps
				if (gl1_overbrightbits->value)
				{
					R_TexEnv(GL_COMBINE);
					glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, gl1_overbrightbits->value);
				}

				R_DrawGLPolyChain(surf->polys,
						(surf->light_s - surf->dlight_s) * (1.0 / (float)gl_state.block_width),
						(surf->light_t - surf->dlight_t) * (1.0 / (float)gl_state.block_height));
			}
		}
	}

	/* restore state */
	glDisable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_TRUE);
}

static void
R_RenderBrushPoly(entity_t *currententity, msurface_t *fa)
{
	int maps;
	qboolean is_dynamic = false;

	c_brush_polys++;

	if (fa->flags & SURF_DRAWTURB)
	{
		/* This is a hack ontop of a hack. Warping surfaces like those generated
		   by R_EmitWaterPolys() don't have a lightmap. Original Quake II therefore
		   negated the global intensity on those surfaces, because otherwise they
		   would show up much too bright. When we implemented overbright bits this
		   hack modified the global GL state in an incompatible way. So implement
		   a new hack, based on overbright bits... Depending on the value set to
		   gl1_overbrightbits the result is different:

		    0: Old behaviour.
		    1: No overbright bits on the global scene but correct lighting on
		       warping surfaces.
		    2: Overbright bits on the global scene but not on warping surfaces.
		        They oversaturate otherwise. */
		/*
		if (gl1_overbrightbits->value)
		{
			R_TexEnv(GL_COMBINE);
			glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, 1);
		}
		else
		{
			R_TexEnv(GL_MODULATE);
			glColor4f(gl_state.inverse_intensity, gl_state.inverse_intensity,
					  gl_state.inverse_intensity, 1.0f);
		}
		*/

		R_EmitWaterPolys(fa);
		// R_TexEnv(GL_REPLACE);

		return;
	}

	// R_TexEnv(GL_REPLACE);

	if (fa->texinfo->flags & SURF_FLOWING)
	{
		R_DrawGLFlowingPoly(fa);
	}
	else
	{
		R_DrawGLPoly(fa->polys);
	}

	if (gl_config.multitexture)
	{
		return;	// lighting already done at this point for mtex
	}

	/* check for lightmap modification */
	for (maps = 0; maps < MAXLIGHTMAPS && fa->styles[maps] != 255; maps++)
	{
		if (r_newrefdef.lightstyles[fa->styles[maps]].white !=
			fa->cached_light[maps])
		{
			goto dynamic;
		}
	}

	/* dynamic this frame or dynamic previously */
	if (fa->dlightframe == r_framecount)
	{
	dynamic:

		if (gl1_dynamic->value)
		{
			if (!(fa->texinfo->flags &
				  (SURF_SKY | SURF_TRANS33 |
				   SURF_TRANS66 | SURF_WARP)))
			{
				is_dynamic = true;
			}
		}
	}

	if (is_dynamic)
	{
		if (maps < MAXLIGHTMAPS &&
			((fa->styles[maps] >= 32) ||
			 (fa->styles[maps] == 0)) &&
			  (fa->dlightframe != r_framecount))
		{
			unsigned temp[34 * 34];
			int smax, tmax;

			smax = (fa->extents[0] >> 4) + 1;
			tmax = (fa->extents[1] >> 4) + 1;

			R_UpdateGLBuffer(buf_singletex, gl_state.lightmap_textures + fa->lightmaptexturenum, 0, 0, 1);

			R_BuildLightMap(fa, (void *)temp, smax * 4);
			R_SetCacheState(fa);

			R_Bind(gl_state.lightmap_textures + fa->lightmaptexturenum);	// what to do...

			glTexSubImage2D(GL_TEXTURE_2D, 0, fa->light_s, fa->light_t,
					smax, tmax, GL_LIGHTMAP_FORMAT, GL_UNSIGNED_BYTE, temp);

			fa->lightmapchain = gl_lms.lightmap_surfaces[fa->lightmaptexturenum];
			gl_lms.lightmap_surfaces[fa->lightmaptexturenum] = fa;
		}
		else
		{
			fa->lightmapchain = gl_lms.lightmap_surfaces[0];
			gl_lms.lightmap_surfaces[0] = fa;
		}
	}
	else
	{
		fa->lightmapchain = gl_lms.lightmap_surfaces[fa->lightmaptexturenum];
		gl_lms.lightmap_surfaces[fa->lightmaptexturenum] = fa;
	}
}

/*
 * Draw water surfaces and windows.
 * The BSP tree is waled front to back, so unwinding the chain
 * of alpha_surfaces will draw back to front, giving proper ordering.
 */
void
R_DrawAlphaSurfaces(void)
{
	msurface_t *s;
	// float intens;
	float alpha;	// = 1.0f, last_alpha = 1.0f;

	/* go back to the world matrix */
	glLoadMatrixf(r_world_matrix);

	glEnable(GL_BLEND);
	R_TexEnv(GL_MODULATE);

	/* the textures are prescaled up for a better
	   lighting range, so scale it back down */
	// intens = gl_state.inverse_intensity;

	for (s = r_alpha_surfaces; s; s = s->texturechain)
	{
		if (s->texinfo->flags & SURF_TRANS33)
		{
			// glColor4f(intens, intens, intens, 0.33);
			alpha = 0.33f;
		}
		else if (s->texinfo->flags & SURF_TRANS66)
		{
			// glColor4f(intens, intens, intens, 0.66);
			alpha = 0.66f;
		}
		else
		{
			// glColor4f(intens, intens, intens, 1);
			alpha = 1.0f;
		}

		/*
		if ( gl_state.currenttextures[gl_state.currenttmu] != s->texinfo->image->texnum
			|| alpha != last_alpha )
		{
			R_UpdateGLBuffer(buf_singletex, s->texinfo->image->texnum, 0, 0);
			last_alpha = alpha;
		}
		*/

		// R_Bind(s->texinfo->image->texnum);
		c_brush_polys++;

		// glColor4f(intens, intens, intens, alpha);
		R_UpdateGLBuffer(buf_alpha, s->texinfo->image->texnum, 0, 0, alpha);

		if (s->flags & SURF_DRAWTURB)
		{
			R_EmitWaterPolys(s);
		}
		else if (s->texinfo->flags & SURF_FLOWING)
		{
			R_DrawGLFlowingPoly(s);
		}
		else
		{
			R_DrawGLPoly(s->polys);
		}
	}
	// Flush the last batched array
	R_ApplyGLBuffer();

	R_TexEnv(GL_REPLACE);
	glColor4f(1, 1, 1, 1);
	glDisable(GL_BLEND);

	r_alpha_surfaces = NULL;
}

static qboolean
R_HasDynamicLights(msurface_t *surf, int *mapNum)
{
	int map;
	qboolean is_dynamic = false;

	if ( r_fullbright->value || !gl1_dynamic->value ||
		(surf->texinfo->flags & (SURF_SKY | SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)) )
	{
		return false;
	}

	// Any dynamic lights on this surface?
	for (map = 0; map < MAXLIGHTMAPS && surf->styles[map] != 255; map++)
	{
		if (r_newrefdef.lightstyles[surf->styles[map]].white != surf->cached_light[map])
		{
			is_dynamic = true;
			break;
		}
	}

	// No matter if it is this frame or was in the previous one: has dynamic lights
	if ( !is_dynamic && ( surf->dlightframe == r_framecount ||
		( surf->dirty_lightmap && !gl_config.lightmapcopies ) ) )
	{
		is_dynamic = true;
	}

	if (mapNum)
	{
		*mapNum = map;
	}
	return is_dynamic;
}

static void
R_UpdateSurfCache(msurface_t *surf, int map)
{
	if ( ((surf->styles[map] >= 32) || (surf->styles[map] == 0))
		&& (surf->dlightframe != r_framecount) )
	{
		R_SetCacheState(surf);
	}

	if (!gl_config.lightmapcopies)
	{
		surf->dirty_lightmap = (surf->dlightframe == r_framecount);
	}
}

static void
R_RenderLightmappedPoly(entity_t *currententity, msurface_t *surf)
{
	int i;
	int j = gl_buf.vtx_ptr * 3;      // vertex index
	int k = gl_buf.vtx_ptr * 2;      // texcoord index
	int nv = surf->polys->numverts;
	// int lmtexture = gl_state.lightmap_textures + surf->lightmaptexturenum;
	float scroll;
	float *v;

	/*
	if (dynamic_frame[surf->lightmaptexturenum])
	{
		lmtexture += gl_state.max_lightmaps;	// bind to dynamic lightmap
	}

	R_MBind(GL_TEXTURE1, lmtexture);
	*/

	// Apply overbrightbits to TMU 1 (lightmap)
	/*
	if (gl1_overbrightbits->value)
	{
		R_TexEnv(GL_COMBINE);
		glTexEnvi(GL_TEXTURE_ENV, GL_RGB_SCALE, gl1_overbrightbits->value);
	}
	*/

	c_brush_polys++;
	v = surf->polys->verts[0];

	if (surf->texinfo->flags & SURF_FLOWING)
	{
		scroll = -64 * ((r_newrefdef.time / 40.0) - (int) (r_newrefdef.time / 40.0));

		if (scroll == 0.0)
		{
			scroll = -64.0;
		}
	}
	else
	{
		scroll = 0.0;
	}

	for (i=0; i < nv-2; i++) {
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr;
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr+i+1;
		gl_buf.idx[gl_buf.idx_ptr++] = gl_buf.vtx_ptr+i+2;
	}

	for (i = 0; i < nv; i++, v += VERTEXSIZE)
	{
		gl_buf.vtx[j] = v[ 0 ];
		gl_buf.vtx[j+1] = v[ 1 ];
		gl_buf.vtx[j+2] = v[ 2 ];
		j += 3;
		gl_buf.tex[0][k] = v[ 3 ] + scroll;
		gl_buf.tex[0][k+1] = v[ 4 ];
		gl_buf.tex[1][k] = v[ 5 ];
		gl_buf.tex[1][k+1] = v[ 6 ];
		k += 2;
	}

	gl_buf.vtx_ptr += nv;

	/*
		YQ2_VLA(GLfloat, tex, 4 * nv);
		unsigned int index_tex = 0;

		for (i = 0; i < nv; i++, v += VERTEXSIZE)
		{
			tex[index_tex++] = v[3] + scroll;
			tex[index_tex++] = v[4];
			tex[index_tex++] = v[5];
			tex[index_tex++] = v[6];
		}
		v = surf->polys->verts[0];

		// Polygon
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, VERTEXSIZE * sizeof(GLfloat), v);

		// Texture
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		qglClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(GLfloat), tex);

		// Lightmap
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		qglClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(GLfloat), tex + 2);

		// Draw the thing
		glDrawArrays(GL_TRIANGLE_FAN, 0, nv);

		YQ2_VLAFREE(tex);
	}
	else
	{
		// Polygon
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, VERTEXSIZE * sizeof(GLfloat), v);

		// Texture
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		qglClientActiveTexture(GL_TEXTURE0);
		glTexCoordPointer(2, GL_FLOAT, VERTEXSIZE * sizeof(GLfloat), v + 3);

		// Lightmap
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		qglClientActiveTexture(GL_TEXTURE1);
		glTexCoordPointer(2, GL_FLOAT, VERTEXSIZE * sizeof(GLfloat), v + 5);

		// Draw it
		glDrawArrays(GL_TRIANGLE_FAN, 0, nv);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	*/
}

/* Upload dynamic lights to each lightmap texture (multitexture path only) */
static void
R_RegenAllLightmaps()
{
	static qboolean changed_last_frame[DYNAMIC_COPIES][MAX_LIGHTMAPS];
	static lmrect_t lmchange[DYNAMIC_COPIES][MAX_LIGHTMAPS];

	int i, map, smax, tmax, top, cc, lmtex, bottom, left, right, bt, bb, bl, br, ut, ub, ul, ur;
	qboolean pixelstore_set = false;
	msurface_t *surf;
	lmrect_t changed;
	byte *base;

	if ( !gl_config.multitexture )
	{
		return;
	}

	cc = 0;
	if (gl_config.lightmapcopies)
	{
		cur_lm_copy = (cur_lm_copy + 1) % DYNAMIC_COPIES;	// alternate between calls
		cc = cur_lm_copy;
	}

	for (i = 1; i < gl_state.max_lightmaps; i++)
	{
		dynamic_frame[i] = false;

		if (!gl_lms.lightmap_surfaces[i] || !gl_lms.lightmap_buffer[0][i])
		{
			continue;
		}

		if (gl_config.lightmapcopies)
		{
			changed = lmchange[cc][i];

			// restore to static lightmap if it has been changed in the past
			if (changed_last_frame[cc][i])
			{
				int offset = (changed.top * gl_state.block_width + changed.left) * LIGHTMAP_BYTES;

				memcpy( gl_lms.lightmap_buffer[cc][i] + offset, gl_lms.lightmap_buffer[DYNAMIC_COPIES][i] + offset,
					( (gl_state.block_width - changed.left) +
					  (gl_state.block_width * (changed.bottom - changed.top - 2)) +
					  changed.right ) * LIGHTMAP_BYTES );

				changed_last_frame[cc][i] = false;
			}
		}

		bt = gl_state.block_height;
		bl = gl_state.block_width;
		bb = br = 0;

		for (surf = gl_lms.lightmap_surfaces[i];
			 surf != 0;
			 surf = surf->lightmapchain)
		{
			if ( !R_HasDynamicLights(surf, &map) )
			{
				continue;
			}

			dynamic_frame[i] = true;
			smax = (surf->extents[0] >> 4) + 1;
			tmax = (surf->extents[1] >> 4) + 1;

			left = surf->light_s;
			right = surf->light_s + smax;
			top = surf->light_t;
			bottom = surf->light_t + tmax;

			base = gl_lms.lightmap_buffer[cc][i];
			base += (top * gl_state.block_width + left) * LIGHTMAP_BYTES;

			R_BuildLightMap(surf, base, gl_state.block_width * LIGHTMAP_BYTES);
			R_UpdateSurfCache(surf, map);

			if (left < bl)
			{
				bl = left;
			}
			if (right > br)
			{
				br = right;
			}
			if (top < bt)
			{
				bt = top;
			}
			if (bottom > bb)
			{
				bb = bottom;
			}
		}

		if (!dynamic_frame[i])
		{
			continue;
		}
		changed_last_frame[cc][i] = true;

		if (!pixelstore_set)
		{
			glPixelStorei(GL_UNPACK_ROW_LENGTH, gl_state.block_width);
			pixelstore_set = true;
		}

		// Obtain the entire area to be updated in the next glTexSubImage2D
		if (gl_config.lightmapcopies)
		{
			// Considers changes in the last frame to be reset with the static lightmap
			// plus the new changes in this frame by dynamic lighting.
			ut = (bt < changed.top)? bt : changed.top;
			ub = (bb > changed.bottom)? bb : changed.bottom;
			ul = (bl < changed.left)? bl : changed.left;
			ur = (br > changed.right)? br : changed.right;

			lmtex = gl_state.max_lightmaps * (cc + 1);
		}
		else
		{
			ut = bt;
			ub = bb;
			ul = bl;
			ur = br;
			lmtex = 0;
		}

		base = gl_lms.lightmap_buffer[cc][i];
		base += (ut * gl_state.block_width + ul) * LIGHTMAP_BYTES;

		// upload changes
		R_Bind(gl_state.lightmap_textures + i + lmtex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, ul, ut, ur - ul, ub - ut,
						GL_LIGHTMAP_FORMAT, GL_UNSIGNED_BYTE, base);

		if (gl_config.lightmapcopies)
		{
			// Changes for next frame(s)
			lmchange[cc][i].top = bt;
			lmchange[cc][i].bottom = bb;
			lmchange[cc][i].left = bl;
			lmchange[cc][i].right = br;
		}
	}

	if (pixelstore_set)
	{
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	}
}

static void
R_DrawTextureChains(entity_t *currententity)
{
	int i;
	msurface_t *s;
	image_t *image;

	c_visible_textures = 0;

	if (!gl_config.multitexture)	// classic path
	{
		for (i = 0, image = gltextures; i < numgltextures; i++, image++)
		{
			if (!image->registration_sequence)
			{
				continue;
			}

			s = image->texturechain;

			if (!s)
			{
				continue;
			}

			c_visible_textures++;

			for ( ; s; s = s->texturechain)
			{
				R_UpdateGLBuffer(buf_singletex, image->texnum, 0, s->flags, 1);
				/*
				if (gl_state.currenttextures[gl_state.currenttmu] != image->texnum)
				{
					// draw everything accumulated in the vertexes
					// and flush it (may be dangerous?)
					R_ApplyGLBuffer();
				}
				*/
				// R_Bind(image->texnum);  // may reset because of dynamic lighting in R_RenderBrushPoly
				R_RenderBrushPoly(currententity, s);
			}

			image->texturechain = NULL;
		}
		// Flush the last batched array
		R_ApplyGLBuffer();
	}
	else	// multitexture
	{
		// R_EnableMultitexture(true);	// no longer needed, R_ApplyGLBuffer() does it now

		for (i = 0, image = gltextures; i < numgltextures; i++, image++)
		{
			if (!image->registration_sequence || !image->texturechain)
			{
				continue;
			}

			// R_MBind(GL_TEXTURE0, image->texnum);	// setting it only once
			c_visible_textures++;

			for (s = image->texturechain; s; s = s->texturechain)
			{
				if (!(s->flags & SURF_DRAWTURB))
				{
					R_UpdateGLBuffer(buf_mtex, image->texnum, s->lightmaptexturenum, 0, 1);
					// R_MBind(GL_TEXTURE1, gl_state.lightmap_textures + s->lightmaptexturenum);	// ?
					R_RenderLightmappedPoly(currententity, s);
				}
			}
		}
		R_ApplyGLBuffer();

		R_EnableMultitexture(false);	// needed to reset status, we may not have SURF_DRAWTURB surfaces in the next loop

		for (i = 0, image = gltextures; i < numgltextures; i++, image++)
		{
			if (!image->registration_sequence || !image->texturechain)
			{
				continue;
			}

			for (s = image->texturechain; s; s = s->texturechain)
			{
				if (s->flags & SURF_DRAWTURB)
				{
					R_UpdateGLBuffer(buf_singletex, image->texnum, 0, s->flags, 1);
					R_RenderBrushPoly(currententity, s);
				}
			}

			image->texturechain = NULL;
		}
		R_ApplyGLBuffer();
	}
}

static void
R_DrawInlineBModel(entity_t *currententity, const model_t *currentmodel)
{
	int i, k;
	cplane_t *pplane;
	float dot;
	msurface_t *psurf;
	dlight_t *lt;
	image_t *image;

	/* calculate dynamic lighting for bmodel */
	if (!gl_config.multitexture && !gl1_flashblend->value)
	{
		lt = r_newrefdef.dlights;

		for (k = 0; k < r_newrefdef.num_dlights; k++, lt++)
		{
			R_MarkLights(lt, 1 << k,
				currentmodel->nodes + currentmodel->firstnode,
				r_dlightframecount, R_MarkSurfaceLights);
		}
	}

	psurf = &currentmodel->surfaces[currentmodel->firstmodelsurface];

	if (currententity->flags & RF_TRANSLUCENT)
	{
		glEnable(GL_BLEND);
		glColor4f(1, 1, 1, 0.25);
		R_TexEnv(GL_MODULATE);
	}

	/* draw texture */
	for (i = 0; i < currentmodel->nummodelsurfaces; i++, psurf++)
	{
		/* find which side of the node we are on */
		pplane = psurf->plane;

		dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

		/* draw the polygon */
		if (((psurf->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON)) ||
			(!(psurf->flags & SURF_PLANEBACK) && (dot > BACKFACE_EPSILON)))
		{
			if (psurf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66))
			{
				/* add to the translucent chain */
				psurf->texturechain = r_alpha_surfaces;
				r_alpha_surfaces = psurf;
			}
			else
			{
				image = R_TextureAnimation(currententity, psurf->texinfo);

				if (gl_config.multitexture && !(psurf->flags & SURF_DRAWTURB))
				{
					// Dynamic lighting already generated in R_GetBrushesLighting()
					R_EnableMultitexture(true);
					R_UpdateGLBuffer(buf_mtex, image->texnum, psurf->lightmaptexturenum, 0, 1);
					// R_MBind(GL_TEXTURE0, image->texnum);
					R_RenderLightmappedPoly(currententity, psurf);
				}
				else
				{
					R_EnableMultitexture(false);
					R_UpdateGLBuffer(buf_singletex, image->texnum, 0, psurf->flags, 1);
					// R_Bind(image->texnum);
					R_RenderBrushPoly(currententity, psurf);
				}
			}
		}
	}
	R_ApplyGLBuffer();	// commenting this ruins everything :(

	if (!(currententity->flags & RF_TRANSLUCENT))
	{
		R_BlendLightmaps(currentmodel);
	}
	else
	{
		glDisable(GL_BLEND);
		glColor4f(1, 1, 1, 1);
		R_TexEnv(GL_REPLACE);
	}
}

void
R_DrawBrushModel(entity_t *currententity, const model_t *currentmodel)
{
	vec3_t mins, maxs;
	int i;
	qboolean rotated;

	if (currentmodel->nummodelsurfaces == 0)
	{
		return;
	}

	// Por qué existe esto? arruina mi intención de agrupar draw calls
	gl_state.currenttextures[0] = gl_state.currenttextures[1] = -1;
	// gl_buf.vtx_ptr = gl_buf.idx_ptr = 0;

	if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2])
	{
		rotated = true;

		for (i = 0; i < 3; i++)
		{
			mins[i] = currententity->origin[i] - currentmodel->radius;
			maxs[i] = currententity->origin[i] + currentmodel->radius;
		}
	}
	else
	{
		rotated = false;
		VectorAdd(currententity->origin, currentmodel->mins, mins);
		VectorAdd(currententity->origin, currentmodel->maxs, maxs);
	}

	if (r_cull->value && R_CullBox(mins, maxs, frustum))
	{
		return;
	}

	if (gl_zfix->value)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
	}

	glColor4f(1, 1, 1, 1);
	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));

	VectorSubtract(r_newrefdef.vieworg, currententity->origin, modelorg);

	if (rotated)
	{
		vec3_t temp;
		vec3_t forward, right, up;

		VectorCopy(modelorg, temp);
		AngleVectors(currententity->angles, forward, right, up);
		modelorg[0] = DotProduct(temp, forward);
		modelorg[1] = -DotProduct(temp, right);
		modelorg[2] = DotProduct(temp, up);
	}

	glPushMatrix();
	currententity->angles[0] = -currententity->angles[0];
	currententity->angles[2] = -currententity->angles[2];
	R_RotateForEntity(currententity);
	currententity->angles[0] = -currententity->angles[0];
	currententity->angles[2] = -currententity->angles[2];

	if (gl_lightmap->value)
	{
		R_TexEnv(GL_REPLACE);
	}
	else
	{
		R_TexEnv(GL_MODULATE);
	}

	R_DrawInlineBModel(currententity, currentmodel);

	glPopMatrix();

	if (gl_zfix->value)
	{
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}

static void
R_RecursiveWorldNode(entity_t *currententity, mnode_t *node)
{
	int c, side, sidebit;
	cplane_t *plane;
	msurface_t *surf, **mark;
	mleaf_t *pleaf;
	float dot;
	image_t *image;

	if (node->contents == CONTENTS_SOLID)
	{
		return; /* solid */
	}

	if (node->visframe != r_visframecount)
	{
		return;
	}

	if (r_cull->value && R_CullBox(node->minmaxs, node->minmaxs + 3, frustum))
	{
		return;
	}

	/* if a leaf node, draw stuff */
	if (node->contents != CONTENTS_NODE)
	{
		pleaf = (mleaf_t *)node;

		/* check for door connected areas */
		if (!R_AreaVisible(r_newrefdef.areabits, pleaf))
			return;	// not visible

		mark = pleaf->firstmarksurface;
		c = pleaf->nummarksurfaces;

		if (c)
		{
			do
			{
				(*mark)->visframe = r_framecount;
				mark++;
			}
			while (--c);
		}

		return;
	}

	/* node is just a decision point, so go down the apropriate
	   sides find which side of the node we are on */
	plane = node->plane;

	switch (plane->type)
	{
		case PLANE_X:
			dot = modelorg[0] - plane->dist;
			break;
		case PLANE_Y:
			dot = modelorg[1] - plane->dist;
			break;
		case PLANE_Z:
			dot = modelorg[2] - plane->dist;
			break;
		default:
			dot = DotProduct(modelorg, plane->normal) - plane->dist;
			break;
	}

	if (dot >= 0)
	{
		side = 0;
		sidebit = 0;
	}
	else
	{
		side = 1;
		sidebit = SURF_PLANEBACK;
	}

	/* recurse down the children, front side first */
	R_RecursiveWorldNode(currententity, node->children[side]);

	/* draw stuff */
	for (c = node->numsurfaces,
		 surf = r_worldmodel->surfaces + node->firstsurface;
		 c; c--, surf++)
	{
		if (surf->visframe != r_framecount)
		{
			continue;
		}

		if ((surf->flags & SURF_PLANEBACK) != sidebit)
		{
			continue; /* wrong side */
		}

		if (surf->texinfo->flags & SURF_SKY)
		{
			/* just adds to visible sky bounds */
			R_AddSkySurface(surf);
		}
		else if (surf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66))
		{
			/* add to the translucent chain */
			surf->texturechain = r_alpha_surfaces;
			r_alpha_surfaces = surf;
			r_alpha_surfaces->texinfo->image = R_TextureAnimation(currententity, surf->texinfo);
		}
		else
		{
			/* the polygon is visible, so add it to the texture sorted chain */
			image = R_TextureAnimation(currententity, surf->texinfo);
			surf->texturechain = image->texturechain;
			image->texturechain = surf;

			if (gl_config.multitexture && !(surf->texinfo->flags & SURF_WARP))	// needed for R_RegenAllLightmaps()
			{
				surf->lightmapchain = gl_lms.lightmap_surfaces[surf->lightmaptexturenum];
				gl_lms.lightmap_surfaces[surf->lightmaptexturenum] = surf;
			}
		}
	}

	/* recurse down the back side */
	R_RecursiveWorldNode(currententity, node->children[!side]);
}

/*
 * This is for the RegenAllLightmaps() function to be able to
 * regenerate lighting not only for the world, but also for
 * the brushes in the entity list.
 * Logic extracted from R_DrawBrushModel() & R_DrawInlineBModel()
 */
static void
R_GetBrushesLighting(void)
{
	int i, k;
	vec3_t mins, maxs;
	msurface_t *surf;
	cplane_t *pplane;
	dlight_t *lt;
	float dot;

	if (!gl_config.multitexture || !r_drawentities->value || gl1_flashblend->value)
	{
		return;
	}

	for (i = 0; i < r_newrefdef.num_entities; i++)
	{
		entity_t *currententity = &r_newrefdef.entities[i];

		if (currententity->flags & RF_BEAM)
		{
			continue;
		}

		const model_t *currentmodel = currententity->model;

		if (!currentmodel || currentmodel->type != mod_brush || currentmodel->nummodelsurfaces == 0)
		{
			continue;
		}
		// R_DrawBrushModel(currententity, currentmodel);

		if (currententity->angles[0] || currententity->angles[1] || currententity->angles[2])
		{
			for (k = 0; k < 3; k++)
			{
				mins[k] = currententity->origin[k] - currentmodel->radius;
				maxs[k] = currententity->origin[k] + currentmodel->radius;
			}
		}
		else
		{
			VectorAdd(currententity->origin, currentmodel->mins, mins);
			VectorAdd(currententity->origin, currentmodel->maxs, maxs);
		}

		if (r_cull->value && R_CullBox(mins, maxs, frustum))
		{
			continue;
		}

		// R_DrawInlineBModel()
		lt = r_newrefdef.dlights;

		for (k = 0; k < r_newrefdef.num_dlights; k++, lt++)
		{
			R_MarkLights(lt, 1 << k,
				currentmodel->nodes + currentmodel->firstnode,
				r_dlightframecount, R_MarkSurfaceLights);
		}

		surf = &currentmodel->surfaces[currentmodel->firstmodelsurface];

		for (k = 0; k < currentmodel->nummodelsurfaces; k++, surf++)
		{
			if (surf->texinfo->flags & (SURF_TRANS33 | SURF_TRANS66 | SURF_WARP)
				|| surf->flags & SURF_DRAWTURB)
			{
				continue;
			}

			// find which side of the node we are on
			pplane = surf->plane;
			dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

			if (((surf->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON)) ||
				(!(surf->flags & SURF_PLANEBACK) && (dot > BACKFACE_EPSILON)))
			{
				surf->lightmapchain = gl_lms.lightmap_surfaces[surf->lightmaptexturenum];
				gl_lms.lightmap_surfaces[surf->lightmaptexturenum] = surf;
			}
		}
	}
}

void
R_DrawWorld(void)
{
	entity_t ent;

	if (!r_drawworld->value)
	{
		return;
	}

	if (r_newrefdef.rdflags & RDF_NOWORLDMODEL)
	{
		return;
	}

	VectorCopy(r_newrefdef.vieworg, modelorg);

	/* auto cycle the world frame for texture animation */
	memset(&ent, 0, sizeof(ent));
	ent.frame = (int)(r_newrefdef.time * 2);

	gl_state.currenttextures[0] = gl_state.currenttextures[1] = -1;
	gl_buf.currenttexture[0] = gl_buf.currenttexture[1] = 0;
	gl_buf.vtx_ptr = gl_buf.idx_ptr = 0;

	glColor4f(1, 1, 1, 1);
	memset(gl_lms.lightmap_surfaces, 0, sizeof(gl_lms.lightmap_surfaces));

	R_ClearSkyBox();
	R_RecursiveWorldNode(&ent, r_worldmodel->nodes);
	R_GetBrushesLighting();
	R_RegenAllLightmaps();
	R_DrawTextureChains(&ent);
	R_BlendLightmaps(r_worldmodel);
	R_DrawSkyBox();
	R_DrawTriangleOutlines();
}

/*
 * Mark the leaves and nodes that are
 * in the PVS for the current cluster
 */
void
R_MarkLeaves(void)
{
	const byte *vis;
	YQ2_ALIGNAS_TYPE(int) byte fatvis[MAX_MAP_LEAFS / 8];
	mnode_t *node;
	int i, c;
	mleaf_t *leaf;
	int cluster;

	if ((r_oldviewcluster == r_viewcluster) &&
		(r_oldviewcluster2 == r_viewcluster2) &&
		!r_novis->value &&
		(r_viewcluster != -1))
	{
		return;
	}

	/* development aid to let you run around
	   and see exactly where the pvs ends */
	if (r_lockpvs->value)
	{
		return;
	}

	r_visframecount++;
	r_oldviewcluster = r_viewcluster;
	r_oldviewcluster2 = r_viewcluster2;

	if (r_novis->value || (r_viewcluster == -1) || !r_worldmodel->vis)
	{
		/* mark everything */
		for (i = 0; i < r_worldmodel->numleafs; i++)
		{
			r_worldmodel->leafs[i].visframe = r_visframecount;
		}

		for (i = 0; i < r_worldmodel->numnodes; i++)
		{
			r_worldmodel->nodes[i].visframe = r_visframecount;
		}

		return;
	}

	vis = Mod_ClusterPVS(r_viewcluster, r_worldmodel);

	/* may have to combine two clusters because of solid water boundaries */
	if (r_viewcluster2 != r_viewcluster)
	{
		memcpy(fatvis, vis, (r_worldmodel->numleafs + 7) / 8);
		vis = Mod_ClusterPVS(r_viewcluster2, r_worldmodel);
		c = (r_worldmodel->numleafs + 31) / 32;

		for (i = 0; i < c; i++)
		{
			((int *)fatvis)[i] |= ((int *)vis)[i];
		}

		vis = fatvis;
	}

	for (i = 0, leaf = r_worldmodel->leafs;
		 i < r_worldmodel->numleafs;
		 i++, leaf++)
	{
		cluster = leaf->cluster;

		if (cluster == -1)
		{
			continue;
		}

		if (vis[cluster >> 3] & (1 << (cluster & 7)))
		{
			node = (mnode_t *)leaf;

			do
			{
				if (node->visframe == r_visframecount)
				{
					break;
				}

				node->visframe = r_visframecount;
				node = node->parent;
			}
			while (node);
		}
	}
}
