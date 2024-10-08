/*

    OpenGL ES loader generated by glad 0.1.36 on Sun Aug 11 03:16:24 2024.

    Language/Generator: C/C++
    Specification: gl
    APIs: gles1=1.0
    Profile: core
    Extensions:
        GL_EXT_discard_framebuffer,
        GL_OES_framebuffer_object,
        GL_OES_texture_npot
    Loader: False
    Local files: False
    Omit khrplatform: False
    Reproducible: False

    Commandline:
        --profile="core" --api="gles1=1.0" --generator="c" --spec="gl" --no-loader --extensions="GL_EXT_discard_framebuffer,GL_OES_framebuffer_object,GL_OES_texture_npot"
    Online:
        https://glad.dav1d.de/#profile=core&language=c&specification=gl&api=gles1%3D1.0&extensions=GL_EXT_discard_framebuffer&extensions=GL_OES_framebuffer_object&extensions=GL_OES_texture_npot
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glad/glad.h>

struct gladGLversionStruct GLVersion = { 0, 0 };

#if defined(GL_ES_VERSION_3_0) || defined(GL_VERSION_3_0)
#define _GLAD_IS_SOME_NEW_VERSION 1
#endif

static int max_loaded_major;
static int max_loaded_minor;

static const char *exts = NULL;
static int num_exts_i = 0;
static char **exts_i = NULL;

static int get_exts(void) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        exts = (const char *)glGetString(GL_EXTENSIONS);
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int index;

        num_exts_i = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (char **)malloc((size_t)num_exts_i * (sizeof *exts_i));
        }

        if (exts_i == NULL) {
            return 0;
        }

        for(index = 0; index < num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = (char*)malloc((len+1) * sizeof(char));
            if(local_str != NULL) {
                memcpy(local_str, gl_str_tmp, (len+1) * sizeof(char));
            }
            exts_i[index] = local_str;
        }
    }
#endif
    return 1;
}

static void free_exts(void) {
    if (exts_i != NULL) {
        int index;
        for(index = 0; index < num_exts_i; index++) {
            free((char *)exts_i[index]);
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}

static int has_ext(const char *ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }

        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }

            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int index;
        if(exts_i == NULL) return 0;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];

            if(exts_i[index] != NULL && strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
#endif

    return 0;
}
int GLAD_GL_VERSION_ES_CM_1_0 = 0;
PFNGLACTIVETEXTUREPROC glad_glActiveTexture = NULL;
PFNGLALPHAFUNCPROC glad_glAlphaFunc = NULL;
PFNGLALPHAFUNCXPROC glad_glAlphaFuncx = NULL;
PFNGLBINDBUFFERPROC glad_glBindBuffer = NULL;
PFNGLBINDTEXTUREPROC glad_glBindTexture = NULL;
PFNGLBLENDFUNCPROC glad_glBlendFunc = NULL;
PFNGLBUFFERDATAPROC glad_glBufferData = NULL;
PFNGLBUFFERSUBDATAPROC glad_glBufferSubData = NULL;
PFNGLCLEARPROC glad_glClear = NULL;
PFNGLCLEARCOLORPROC glad_glClearColor = NULL;
PFNGLCLEARCOLORXPROC glad_glClearColorx = NULL;
PFNGLCLEARDEPTHFPROC glad_glClearDepthf = NULL;
PFNGLCLEARDEPTHXPROC glad_glClearDepthx = NULL;
PFNGLCLEARSTENCILPROC glad_glClearStencil = NULL;
PFNGLCLIENTACTIVETEXTUREPROC glad_glClientActiveTexture = NULL;
PFNGLCLIPPLANEFPROC glad_glClipPlanef = NULL;
PFNGLCLIPPLANEXPROC glad_glClipPlanex = NULL;
PFNGLCOLOR4FPROC glad_glColor4f = NULL;
PFNGLCOLOR4UBPROC glad_glColor4ub = NULL;
PFNGLCOLOR4XPROC glad_glColor4x = NULL;
PFNGLCOLORMASKPROC glad_glColorMask = NULL;
PFNGLCOLORPOINTERPROC glad_glColorPointer = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glad_glCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glad_glCompressedTexSubImage2D = NULL;
PFNGLCOPYTEXIMAGE2DPROC glad_glCopyTexImage2D = NULL;
PFNGLCOPYTEXSUBIMAGE2DPROC glad_glCopyTexSubImage2D = NULL;
PFNGLCULLFACEPROC glad_glCullFace = NULL;
PFNGLDELETEBUFFERSPROC glad_glDeleteBuffers = NULL;
PFNGLDELETETEXTURESPROC glad_glDeleteTextures = NULL;
PFNGLDEPTHFUNCPROC glad_glDepthFunc = NULL;
PFNGLDEPTHMASKPROC glad_glDepthMask = NULL;
PFNGLDEPTHRANGEFPROC glad_glDepthRangef = NULL;
PFNGLDEPTHRANGEXPROC glad_glDepthRangex = NULL;
PFNGLDISABLEPROC glad_glDisable = NULL;
PFNGLDISABLECLIENTSTATEPROC glad_glDisableClientState = NULL;
PFNGLDRAWARRAYSPROC glad_glDrawArrays = NULL;
PFNGLDRAWELEMENTSPROC glad_glDrawElements = NULL;
PFNGLENABLEPROC glad_glEnable = NULL;
PFNGLENABLECLIENTSTATEPROC glad_glEnableClientState = NULL;
PFNGLFINISHPROC glad_glFinish = NULL;
PFNGLFLUSHPROC glad_glFlush = NULL;
PFNGLFOGFPROC glad_glFogf = NULL;
PFNGLFOGFVPROC glad_glFogfv = NULL;
PFNGLFOGXPROC glad_glFogx = NULL;
PFNGLFOGXVPROC glad_glFogxv = NULL;
PFNGLFRONTFACEPROC glad_glFrontFace = NULL;
PFNGLFRUSTUMFPROC glad_glFrustumf = NULL;
PFNGLFRUSTUMXPROC glad_glFrustumx = NULL;
PFNGLGENBUFFERSPROC glad_glGenBuffers = NULL;
PFNGLGENTEXTURESPROC glad_glGenTextures = NULL;
PFNGLGETBOOLEANVPROC glad_glGetBooleanv = NULL;
PFNGLGETBUFFERPARAMETERIVPROC glad_glGetBufferParameteriv = NULL;
PFNGLGETCLIPPLANEFPROC glad_glGetClipPlanef = NULL;
PFNGLGETCLIPPLANEXPROC glad_glGetClipPlanex = NULL;
PFNGLGETERRORPROC glad_glGetError = NULL;
PFNGLGETFIXEDVPROC glad_glGetFixedv = NULL;
PFNGLGETFLOATVPROC glad_glGetFloatv = NULL;
PFNGLGETINTEGERVPROC glad_glGetIntegerv = NULL;
PFNGLGETLIGHTFVPROC glad_glGetLightfv = NULL;
PFNGLGETLIGHTXVPROC glad_glGetLightxv = NULL;
PFNGLGETMATERIALFVPROC glad_glGetMaterialfv = NULL;
PFNGLGETMATERIALXVPROC glad_glGetMaterialxv = NULL;
PFNGLGETPOINTERVPROC glad_glGetPointerv = NULL;
PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLGETTEXENVFVPROC glad_glGetTexEnvfv = NULL;
PFNGLGETTEXENVIVPROC glad_glGetTexEnviv = NULL;
PFNGLGETTEXENVXVPROC glad_glGetTexEnvxv = NULL;
PFNGLGETTEXPARAMETERFVPROC glad_glGetTexParameterfv = NULL;
PFNGLGETTEXPARAMETERIVPROC glad_glGetTexParameteriv = NULL;
PFNGLGETTEXPARAMETERXVPROC glad_glGetTexParameterxv = NULL;
PFNGLHINTPROC glad_glHint = NULL;
PFNGLISBUFFERPROC glad_glIsBuffer = NULL;
PFNGLISENABLEDPROC glad_glIsEnabled = NULL;
PFNGLISTEXTUREPROC glad_glIsTexture = NULL;
PFNGLLIGHTMODELFPROC glad_glLightModelf = NULL;
PFNGLLIGHTMODELFVPROC glad_glLightModelfv = NULL;
PFNGLLIGHTMODELXPROC glad_glLightModelx = NULL;
PFNGLLIGHTMODELXVPROC glad_glLightModelxv = NULL;
PFNGLLIGHTFPROC glad_glLightf = NULL;
PFNGLLIGHTFVPROC glad_glLightfv = NULL;
PFNGLLIGHTXPROC glad_glLightx = NULL;
PFNGLLIGHTXVPROC glad_glLightxv = NULL;
PFNGLLINEWIDTHPROC glad_glLineWidth = NULL;
PFNGLLINEWIDTHXPROC glad_glLineWidthx = NULL;
PFNGLLOADIDENTITYPROC glad_glLoadIdentity = NULL;
PFNGLLOADMATRIXFPROC glad_glLoadMatrixf = NULL;
PFNGLLOADMATRIXXPROC glad_glLoadMatrixx = NULL;
PFNGLLOGICOPPROC glad_glLogicOp = NULL;
PFNGLMATERIALFPROC glad_glMaterialf = NULL;
PFNGLMATERIALFVPROC glad_glMaterialfv = NULL;
PFNGLMATERIALXPROC glad_glMaterialx = NULL;
PFNGLMATERIALXVPROC glad_glMaterialxv = NULL;
PFNGLMATRIXMODEPROC glad_glMatrixMode = NULL;
PFNGLMULTMATRIXFPROC glad_glMultMatrixf = NULL;
PFNGLMULTMATRIXXPROC glad_glMultMatrixx = NULL;
PFNGLMULTITEXCOORD4FPROC glad_glMultiTexCoord4f = NULL;
PFNGLMULTITEXCOORD4XPROC glad_glMultiTexCoord4x = NULL;
PFNGLNORMAL3FPROC glad_glNormal3f = NULL;
PFNGLNORMAL3XPROC glad_glNormal3x = NULL;
PFNGLNORMALPOINTERPROC glad_glNormalPointer = NULL;
PFNGLORTHOFPROC glad_glOrthof = NULL;
PFNGLORTHOXPROC glad_glOrthox = NULL;
PFNGLPIXELSTOREIPROC glad_glPixelStorei = NULL;
PFNGLPOINTPARAMETERFPROC glad_glPointParameterf = NULL;
PFNGLPOINTPARAMETERFVPROC glad_glPointParameterfv = NULL;
PFNGLPOINTPARAMETERXPROC glad_glPointParameterx = NULL;
PFNGLPOINTPARAMETERXVPROC glad_glPointParameterxv = NULL;
PFNGLPOINTSIZEPROC glad_glPointSize = NULL;
PFNGLPOINTSIZEXPROC glad_glPointSizex = NULL;
PFNGLPOLYGONOFFSETPROC glad_glPolygonOffset = NULL;
PFNGLPOLYGONOFFSETXPROC glad_glPolygonOffsetx = NULL;
PFNGLPOPMATRIXPROC glad_glPopMatrix = NULL;
PFNGLPUSHMATRIXPROC glad_glPushMatrix = NULL;
PFNGLREADPIXELSPROC glad_glReadPixels = NULL;
PFNGLROTATEFPROC glad_glRotatef = NULL;
PFNGLROTATEXPROC glad_glRotatex = NULL;
PFNGLSAMPLECOVERAGEPROC glad_glSampleCoverage = NULL;
PFNGLSAMPLECOVERAGEXPROC glad_glSampleCoveragex = NULL;
PFNGLSCALEFPROC glad_glScalef = NULL;
PFNGLSCALEXPROC glad_glScalex = NULL;
PFNGLSCISSORPROC glad_glScissor = NULL;
PFNGLSHADEMODELPROC glad_glShadeModel = NULL;
PFNGLSTENCILFUNCPROC glad_glStencilFunc = NULL;
PFNGLSTENCILMASKPROC glad_glStencilMask = NULL;
PFNGLSTENCILOPPROC glad_glStencilOp = NULL;
PFNGLTEXCOORDPOINTERPROC glad_glTexCoordPointer = NULL;
PFNGLTEXENVFPROC glad_glTexEnvf = NULL;
PFNGLTEXENVFVPROC glad_glTexEnvfv = NULL;
PFNGLTEXENVIPROC glad_glTexEnvi = NULL;
PFNGLTEXENVIVPROC glad_glTexEnviv = NULL;
PFNGLTEXENVXPROC glad_glTexEnvx = NULL;
PFNGLTEXENVXVPROC glad_glTexEnvxv = NULL;
PFNGLTEXIMAGE2DPROC glad_glTexImage2D = NULL;
PFNGLTEXPARAMETERFPROC glad_glTexParameterf = NULL;
PFNGLTEXPARAMETERFVPROC glad_glTexParameterfv = NULL;
PFNGLTEXPARAMETERIPROC glad_glTexParameteri = NULL;
PFNGLTEXPARAMETERIVPROC glad_glTexParameteriv = NULL;
PFNGLTEXPARAMETERXPROC glad_glTexParameterx = NULL;
PFNGLTEXPARAMETERXVPROC glad_glTexParameterxv = NULL;
PFNGLTEXSUBIMAGE2DPROC glad_glTexSubImage2D = NULL;
PFNGLTRANSLATEFPROC glad_glTranslatef = NULL;
PFNGLTRANSLATEXPROC glad_glTranslatex = NULL;
PFNGLVERTEXPOINTERPROC glad_glVertexPointer = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;
int GLAD_GL_EXT_discard_framebuffer = 0;
int GLAD_GL_OES_framebuffer_object = 0;
int GLAD_GL_OES_texture_npot = 0;
PFNGLDISCARDFRAMEBUFFEREXTPROC glad_glDiscardFramebufferEXT = NULL;
PFNGLISRENDERBUFFEROESPROC glad_glIsRenderbufferOES = NULL;
PFNGLBINDRENDERBUFFEROESPROC glad_glBindRenderbufferOES = NULL;
PFNGLDELETERENDERBUFFERSOESPROC glad_glDeleteRenderbuffersOES = NULL;
PFNGLGENRENDERBUFFERSOESPROC glad_glGenRenderbuffersOES = NULL;
PFNGLRENDERBUFFERSTORAGEOESPROC glad_glRenderbufferStorageOES = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVOESPROC glad_glGetRenderbufferParameterivOES = NULL;
PFNGLISFRAMEBUFFEROESPROC glad_glIsFramebufferOES = NULL;
PFNGLBINDFRAMEBUFFEROESPROC glad_glBindFramebufferOES = NULL;
PFNGLDELETEFRAMEBUFFERSOESPROC glad_glDeleteFramebuffersOES = NULL;
PFNGLGENFRAMEBUFFERSOESPROC glad_glGenFramebuffersOES = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSOESPROC glad_glCheckFramebufferStatusOES = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEROESPROC glad_glFramebufferRenderbufferOES = NULL;
PFNGLFRAMEBUFFERTEXTURE2DOESPROC glad_glFramebufferTexture2DOES = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC glad_glGetFramebufferAttachmentParameterivOES = NULL;
PFNGLGENERATEMIPMAPOESPROC glad_glGenerateMipmapOES = NULL;
static void load_GL_VERSION_ES_CM_1_0(GLADloadproc load) {
	if(!GLAD_GL_VERSION_ES_CM_1_0) return;
	glad_glAlphaFunc = (PFNGLALPHAFUNCPROC)load("glAlphaFunc");
	glad_glClearColor = (PFNGLCLEARCOLORPROC)load("glClearColor");
	glad_glClearDepthf = (PFNGLCLEARDEPTHFPROC)load("glClearDepthf");
	glad_glClipPlanef = (PFNGLCLIPPLANEFPROC)load("glClipPlanef");
	glad_glColor4f = (PFNGLCOLOR4FPROC)load("glColor4f");
	glad_glDepthRangef = (PFNGLDEPTHRANGEFPROC)load("glDepthRangef");
	glad_glFogf = (PFNGLFOGFPROC)load("glFogf");
	glad_glFogfv = (PFNGLFOGFVPROC)load("glFogfv");
	glad_glFrustumf = (PFNGLFRUSTUMFPROC)load("glFrustumf");
	glad_glGetClipPlanef = (PFNGLGETCLIPPLANEFPROC)load("glGetClipPlanef");
	glad_glGetFloatv = (PFNGLGETFLOATVPROC)load("glGetFloatv");
	glad_glGetLightfv = (PFNGLGETLIGHTFVPROC)load("glGetLightfv");
	glad_glGetMaterialfv = (PFNGLGETMATERIALFVPROC)load("glGetMaterialfv");
	glad_glGetTexEnvfv = (PFNGLGETTEXENVFVPROC)load("glGetTexEnvfv");
	glad_glGetTexParameterfv = (PFNGLGETTEXPARAMETERFVPROC)load("glGetTexParameterfv");
	glad_glLightModelf = (PFNGLLIGHTMODELFPROC)load("glLightModelf");
	glad_glLightModelfv = (PFNGLLIGHTMODELFVPROC)load("glLightModelfv");
	glad_glLightf = (PFNGLLIGHTFPROC)load("glLightf");
	glad_glLightfv = (PFNGLLIGHTFVPROC)load("glLightfv");
	glad_glLineWidth = (PFNGLLINEWIDTHPROC)load("glLineWidth");
	glad_glLoadMatrixf = (PFNGLLOADMATRIXFPROC)load("glLoadMatrixf");
	glad_glMaterialf = (PFNGLMATERIALFPROC)load("glMaterialf");
	glad_glMaterialfv = (PFNGLMATERIALFVPROC)load("glMaterialfv");
	glad_glMultMatrixf = (PFNGLMULTMATRIXFPROC)load("glMultMatrixf");
	glad_glMultiTexCoord4f = (PFNGLMULTITEXCOORD4FPROC)load("glMultiTexCoord4f");
	glad_glNormal3f = (PFNGLNORMAL3FPROC)load("glNormal3f");
	glad_glOrthof = (PFNGLORTHOFPROC)load("glOrthof");
	glad_glPointParameterf = (PFNGLPOINTPARAMETERFPROC)load("glPointParameterf");
	glad_glPointParameterfv = (PFNGLPOINTPARAMETERFVPROC)load("glPointParameterfv");
	glad_glPointSize = (PFNGLPOINTSIZEPROC)load("glPointSize");
	glad_glPolygonOffset = (PFNGLPOLYGONOFFSETPROC)load("glPolygonOffset");
	glad_glRotatef = (PFNGLROTATEFPROC)load("glRotatef");
	glad_glScalef = (PFNGLSCALEFPROC)load("glScalef");
	glad_glTexEnvf = (PFNGLTEXENVFPROC)load("glTexEnvf");
	glad_glTexEnvfv = (PFNGLTEXENVFVPROC)load("glTexEnvfv");
	glad_glTexParameterf = (PFNGLTEXPARAMETERFPROC)load("glTexParameterf");
	glad_glTexParameterfv = (PFNGLTEXPARAMETERFVPROC)load("glTexParameterfv");
	glad_glTranslatef = (PFNGLTRANSLATEFPROC)load("glTranslatef");
	glad_glActiveTexture = (PFNGLACTIVETEXTUREPROC)load("glActiveTexture");
	glad_glAlphaFuncx = (PFNGLALPHAFUNCXPROC)load("glAlphaFuncx");
	glad_glBindBuffer = (PFNGLBINDBUFFERPROC)load("glBindBuffer");
	glad_glBindTexture = (PFNGLBINDTEXTUREPROC)load("glBindTexture");
	glad_glBlendFunc = (PFNGLBLENDFUNCPROC)load("glBlendFunc");
	glad_glBufferData = (PFNGLBUFFERDATAPROC)load("glBufferData");
	glad_glBufferSubData = (PFNGLBUFFERSUBDATAPROC)load("glBufferSubData");
	glad_glClear = (PFNGLCLEARPROC)load("glClear");
	glad_glClearColorx = (PFNGLCLEARCOLORXPROC)load("glClearColorx");
	glad_glClearDepthx = (PFNGLCLEARDEPTHXPROC)load("glClearDepthx");
	glad_glClearStencil = (PFNGLCLEARSTENCILPROC)load("glClearStencil");
	glad_glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)load("glClientActiveTexture");
	glad_glClipPlanex = (PFNGLCLIPPLANEXPROC)load("glClipPlanex");
	glad_glColor4ub = (PFNGLCOLOR4UBPROC)load("glColor4ub");
	glad_glColor4x = (PFNGLCOLOR4XPROC)load("glColor4x");
	glad_glColorMask = (PFNGLCOLORMASKPROC)load("glColorMask");
	glad_glColorPointer = (PFNGLCOLORPOINTERPROC)load("glColorPointer");
	glad_glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)load("glCompressedTexImage2D");
	glad_glCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)load("glCompressedTexSubImage2D");
	glad_glCopyTexImage2D = (PFNGLCOPYTEXIMAGE2DPROC)load("glCopyTexImage2D");
	glad_glCopyTexSubImage2D = (PFNGLCOPYTEXSUBIMAGE2DPROC)load("glCopyTexSubImage2D");
	glad_glCullFace = (PFNGLCULLFACEPROC)load("glCullFace");
	glad_glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)load("glDeleteBuffers");
	glad_glDeleteTextures = (PFNGLDELETETEXTURESPROC)load("glDeleteTextures");
	glad_glDepthFunc = (PFNGLDEPTHFUNCPROC)load("glDepthFunc");
	glad_glDepthMask = (PFNGLDEPTHMASKPROC)load("glDepthMask");
	glad_glDepthRangex = (PFNGLDEPTHRANGEXPROC)load("glDepthRangex");
	glad_glDisable = (PFNGLDISABLEPROC)load("glDisable");
	glad_glDisableClientState = (PFNGLDISABLECLIENTSTATEPROC)load("glDisableClientState");
	glad_glDrawArrays = (PFNGLDRAWARRAYSPROC)load("glDrawArrays");
	glad_glDrawElements = (PFNGLDRAWELEMENTSPROC)load("glDrawElements");
	glad_glEnable = (PFNGLENABLEPROC)load("glEnable");
	glad_glEnableClientState = (PFNGLENABLECLIENTSTATEPROC)load("glEnableClientState");
	glad_glFinish = (PFNGLFINISHPROC)load("glFinish");
	glad_glFlush = (PFNGLFLUSHPROC)load("glFlush");
	glad_glFogx = (PFNGLFOGXPROC)load("glFogx");
	glad_glFogxv = (PFNGLFOGXVPROC)load("glFogxv");
	glad_glFrontFace = (PFNGLFRONTFACEPROC)load("glFrontFace");
	glad_glFrustumx = (PFNGLFRUSTUMXPROC)load("glFrustumx");
	glad_glGetBooleanv = (PFNGLGETBOOLEANVPROC)load("glGetBooleanv");
	glad_glGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIVPROC)load("glGetBufferParameteriv");
	glad_glGetClipPlanex = (PFNGLGETCLIPPLANEXPROC)load("glGetClipPlanex");
	glad_glGenBuffers = (PFNGLGENBUFFERSPROC)load("glGenBuffers");
	glad_glGenTextures = (PFNGLGENTEXTURESPROC)load("glGenTextures");
	glad_glGetError = (PFNGLGETERRORPROC)load("glGetError");
	glad_glGetFixedv = (PFNGLGETFIXEDVPROC)load("glGetFixedv");
	glad_glGetIntegerv = (PFNGLGETINTEGERVPROC)load("glGetIntegerv");
	glad_glGetLightxv = (PFNGLGETLIGHTXVPROC)load("glGetLightxv");
	glad_glGetMaterialxv = (PFNGLGETMATERIALXVPROC)load("glGetMaterialxv");
	glad_glGetPointerv = (PFNGLGETPOINTERVPROC)load("glGetPointerv");
	glad_glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	glad_glGetTexEnviv = (PFNGLGETTEXENVIVPROC)load("glGetTexEnviv");
	glad_glGetTexEnvxv = (PFNGLGETTEXENVXVPROC)load("glGetTexEnvxv");
	glad_glGetTexParameteriv = (PFNGLGETTEXPARAMETERIVPROC)load("glGetTexParameteriv");
	glad_glGetTexParameterxv = (PFNGLGETTEXPARAMETERXVPROC)load("glGetTexParameterxv");
	glad_glHint = (PFNGLHINTPROC)load("glHint");
	glad_glIsBuffer = (PFNGLISBUFFERPROC)load("glIsBuffer");
	glad_glIsEnabled = (PFNGLISENABLEDPROC)load("glIsEnabled");
	glad_glIsTexture = (PFNGLISTEXTUREPROC)load("glIsTexture");
	glad_glLightModelx = (PFNGLLIGHTMODELXPROC)load("glLightModelx");
	glad_glLightModelxv = (PFNGLLIGHTMODELXVPROC)load("glLightModelxv");
	glad_glLightx = (PFNGLLIGHTXPROC)load("glLightx");
	glad_glLightxv = (PFNGLLIGHTXVPROC)load("glLightxv");
	glad_glLineWidthx = (PFNGLLINEWIDTHXPROC)load("glLineWidthx");
	glad_glLoadIdentity = (PFNGLLOADIDENTITYPROC)load("glLoadIdentity");
	glad_glLoadMatrixx = (PFNGLLOADMATRIXXPROC)load("glLoadMatrixx");
	glad_glLogicOp = (PFNGLLOGICOPPROC)load("glLogicOp");
	glad_glMaterialx = (PFNGLMATERIALXPROC)load("glMaterialx");
	glad_glMaterialxv = (PFNGLMATERIALXVPROC)load("glMaterialxv");
	glad_glMatrixMode = (PFNGLMATRIXMODEPROC)load("glMatrixMode");
	glad_glMultMatrixx = (PFNGLMULTMATRIXXPROC)load("glMultMatrixx");
	glad_glMultiTexCoord4x = (PFNGLMULTITEXCOORD4XPROC)load("glMultiTexCoord4x");
	glad_glNormal3x = (PFNGLNORMAL3XPROC)load("glNormal3x");
	glad_glNormalPointer = (PFNGLNORMALPOINTERPROC)load("glNormalPointer");
	glad_glOrthox = (PFNGLORTHOXPROC)load("glOrthox");
	glad_glPixelStorei = (PFNGLPIXELSTOREIPROC)load("glPixelStorei");
	glad_glPointParameterx = (PFNGLPOINTPARAMETERXPROC)load("glPointParameterx");
	glad_glPointParameterxv = (PFNGLPOINTPARAMETERXVPROC)load("glPointParameterxv");
	glad_glPointSizex = (PFNGLPOINTSIZEXPROC)load("glPointSizex");
	glad_glPolygonOffsetx = (PFNGLPOLYGONOFFSETXPROC)load("glPolygonOffsetx");
	glad_glPopMatrix = (PFNGLPOPMATRIXPROC)load("glPopMatrix");
	glad_glPushMatrix = (PFNGLPUSHMATRIXPROC)load("glPushMatrix");
	glad_glReadPixels = (PFNGLREADPIXELSPROC)load("glReadPixels");
	glad_glRotatex = (PFNGLROTATEXPROC)load("glRotatex");
	glad_glSampleCoverage = (PFNGLSAMPLECOVERAGEPROC)load("glSampleCoverage");
	glad_glSampleCoveragex = (PFNGLSAMPLECOVERAGEXPROC)load("glSampleCoveragex");
	glad_glScalex = (PFNGLSCALEXPROC)load("glScalex");
	glad_glScissor = (PFNGLSCISSORPROC)load("glScissor");
	glad_glShadeModel = (PFNGLSHADEMODELPROC)load("glShadeModel");
	glad_glStencilFunc = (PFNGLSTENCILFUNCPROC)load("glStencilFunc");
	glad_glStencilMask = (PFNGLSTENCILMASKPROC)load("glStencilMask");
	glad_glStencilOp = (PFNGLSTENCILOPPROC)load("glStencilOp");
	glad_glTexCoordPointer = (PFNGLTEXCOORDPOINTERPROC)load("glTexCoordPointer");
	glad_glTexEnvi = (PFNGLTEXENVIPROC)load("glTexEnvi");
	glad_glTexEnvx = (PFNGLTEXENVXPROC)load("glTexEnvx");
	glad_glTexEnviv = (PFNGLTEXENVIVPROC)load("glTexEnviv");
	glad_glTexEnvxv = (PFNGLTEXENVXVPROC)load("glTexEnvxv");
	glad_glTexImage2D = (PFNGLTEXIMAGE2DPROC)load("glTexImage2D");
	glad_glTexParameteri = (PFNGLTEXPARAMETERIPROC)load("glTexParameteri");
	glad_glTexParameterx = (PFNGLTEXPARAMETERXPROC)load("glTexParameterx");
	glad_glTexParameteriv = (PFNGLTEXPARAMETERIVPROC)load("glTexParameteriv");
	glad_glTexParameterxv = (PFNGLTEXPARAMETERXVPROC)load("glTexParameterxv");
	glad_glTexSubImage2D = (PFNGLTEXSUBIMAGE2DPROC)load("glTexSubImage2D");
	glad_glTranslatex = (PFNGLTRANSLATEXPROC)load("glTranslatex");
	glad_glVertexPointer = (PFNGLVERTEXPOINTERPROC)load("glVertexPointer");
	glad_glViewport = (PFNGLVIEWPORTPROC)load("glViewport");
}
static void load_GL_EXT_discard_framebuffer(GLADloadproc load) {
	if(!GLAD_GL_EXT_discard_framebuffer) return;
	glad_glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)load("glDiscardFramebufferEXT");
}
static void load_GL_OES_framebuffer_object(GLADloadproc load) {
	if(!GLAD_GL_OES_framebuffer_object) return;
	glad_glIsRenderbufferOES = (PFNGLISRENDERBUFFEROESPROC)load("glIsRenderbufferOES");
	glad_glBindRenderbufferOES = (PFNGLBINDRENDERBUFFEROESPROC)load("glBindRenderbufferOES");
	glad_glDeleteRenderbuffersOES = (PFNGLDELETERENDERBUFFERSOESPROC)load("glDeleteRenderbuffersOES");
	glad_glGenRenderbuffersOES = (PFNGLGENRENDERBUFFERSOESPROC)load("glGenRenderbuffersOES");
	glad_glRenderbufferStorageOES = (PFNGLRENDERBUFFERSTORAGEOESPROC)load("glRenderbufferStorageOES");
	glad_glGetRenderbufferParameterivOES = (PFNGLGETRENDERBUFFERPARAMETERIVOESPROC)load("glGetRenderbufferParameterivOES");
	glad_glIsFramebufferOES = (PFNGLISFRAMEBUFFEROESPROC)load("glIsFramebufferOES");
	glad_glBindFramebufferOES = (PFNGLBINDFRAMEBUFFEROESPROC)load("glBindFramebufferOES");
	glad_glDeleteFramebuffersOES = (PFNGLDELETEFRAMEBUFFERSOESPROC)load("glDeleteFramebuffersOES");
	glad_glGenFramebuffersOES = (PFNGLGENFRAMEBUFFERSOESPROC)load("glGenFramebuffersOES");
	glad_glCheckFramebufferStatusOES = (PFNGLCHECKFRAMEBUFFERSTATUSOESPROC)load("glCheckFramebufferStatusOES");
	glad_glFramebufferRenderbufferOES = (PFNGLFRAMEBUFFERRENDERBUFFEROESPROC)load("glFramebufferRenderbufferOES");
	glad_glFramebufferTexture2DOES = (PFNGLFRAMEBUFFERTEXTURE2DOESPROC)load("glFramebufferTexture2DOES");
	glad_glGetFramebufferAttachmentParameterivOES = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC)load("glGetFramebufferAttachmentParameterivOES");
	glad_glGenerateMipmapOES = (PFNGLGENERATEMIPMAPOESPROC)load("glGenerateMipmapOES");
}
static int find_extensionsGLES1(void) {
	if (!get_exts()) return 0;
	GLAD_GL_EXT_discard_framebuffer = has_ext("GL_EXT_discard_framebuffer");
	GLAD_GL_OES_framebuffer_object = has_ext("GL_OES_framebuffer_object");
	GLAD_GL_OES_texture_npot = has_ext("GL_OES_texture_npot");
	free_exts();
	return 1;
}

static void find_coreGLES1(void) {

    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    int i, major, minor;

    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    version = (const char*) glGetString(GL_VERSION);
    if (!version) return;

    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &major, &minor);
#endif

    GLVersion.major = major; GLVersion.minor = minor;
    max_loaded_major = major; max_loaded_minor = minor;
	GLAD_GL_VERSION_ES_CM_1_0 = (major == 1 && minor >= 0) || major > 1;
	if (GLVersion.major > 1 || (GLVersion.major >= 1 && GLVersion.minor >= 0)) {
		max_loaded_major = 1;
		max_loaded_minor = 0;
	}
}

int gladLoadGLES1Loader(GLADloadproc load) {
	GLVersion.major = 0; GLVersion.minor = 0;
	glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	if(glGetString == NULL) return 0;
	if(glGetString(GL_VERSION) == NULL) return 0;
	find_coreGLES1();
	load_GL_VERSION_ES_CM_1_0(load);

	if (!find_extensionsGLES1()) return 0;
	load_GL_EXT_discard_framebuffer(load);
	load_GL_OES_framebuffer_object(load);
	return GLVersion.major != 0 || GLVersion.minor != 0;
}

