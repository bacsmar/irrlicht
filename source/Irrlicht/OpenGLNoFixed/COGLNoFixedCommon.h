// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLES2_COMMON_H_INCLUDED__
#define __C_OGLES2_COMMON_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGL_NOFIXED_

#if defined(_IRR_COMPILE_WITH_IOS_DEVICE_)
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#elif defined(_IRR_COMPILE_WITH_ANDROID_DEVICE_)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/eglplatform.h>
#elif defined(_IRR_WINDOWS_API_)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
	#define GL_GLEXT_LEGACY 1
	#endif
	#include <GL/gl.h>
	#if defined(_IRR_OPENGL_USE_EXTPOINTER_)
		#include "../glext.h"
	#endif
		#include "../wglext.h"

	#ifdef _MSC_VER
	#pragma comment(lib, "OpenGL32.lib")
	#endif
#else
#include <GLES2/gl2.h>
#include <EGL/eglplatform.h>
typedef char GLchar;
#if defined(_IRR_OGLES2_USE_EXTPOINTER_)
#include "gles2-ext.h"
#endif
#endif

#ifndef GL_BGRA
#define GL_BGRA 0x80E1;
#endif

// FBO definitions.

//#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER 1
//#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER 2
//#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 3
#ifdef GL_EXT_framebuffer_object
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#else
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS 6
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS 7
#endif

// Irrlicht's OpenGL version.

#define IRR_OPENGL_VERSION 20

namespace irr
{
namespace video
{

	// Forward declarations.

	class COpenGLCoreFeature;

	template <class TOpenGLDriver>
	class COpenGLCoreTexture;

	template <class TOpenGLDriver, class TOpenGLTexture>
	class COpenGLCoreRenderTarget;

	template <class TOpenGLDriver, class TOpenGLTexture>
	class COpenGLCoreCacheHandler;

	class COpenGLNoFixedDriver;
	typedef COpenGLCoreTexture<COpenGLNoFixedDriver> COpenGLTexture;
	typedef COpenGLCoreRenderTarget<COpenGLNoFixedDriver, COpenGLTexture> COpenGLRenderTarget;
	typedef COpenGLCoreCacheHandler<COpenGLNoFixedDriver, COpenGLTexture> COpenGLCacheHandler;

}
}

#endif
#endif
