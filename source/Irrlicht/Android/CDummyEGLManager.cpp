// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
//
// SDL2 upgrade
// Jonathan Frisch
// Sep 15, 2014

#include "CDummyEGLManager.h"

#ifdef _IRR_COMPILE_WITH_DUMMY_EGL_MANAGER_

#include "irrString.h"
#include "os.h"

namespace irr
{
namespace video
{

CDummyEGLManager::CDummyEGLManager() : IContextManager()
{
	#ifdef _DEBUG
	setDebugName("CDummyEGLManager");
	#endif
}

CDummyEGLManager::~CDummyEGLManager()
{
    destroyContext();
    destroySurface();
    terminate();
}

bool CDummyEGLManager::initialize(const SIrrlichtCreationParameters& params, const SExposedVideoData& data)
{	
	os::Printer::log("CDummyEGLManager::initialize", ELL_INFORMATION);
	Params=params;
	Data=data;    
	return true;	// bypass
}

void CDummyEGLManager::terminate()
{
	os::Printer::log("CDummyEGLManager::terminate", ELL_INFORMATION);
}

bool CDummyEGLManager::generateSurface()
{
    return true;
}

void CDummyEGLManager::destroySurface()
{
	os::Printer::log("CDummyEGLManager::destroySurface", ELL_INFORMATION);
}

bool CDummyEGLManager::generateContext()
{ 
	return true;
}

void CDummyEGLManager::destroyContext()
{
	os::Printer::log("CDummyEGLManager::destroyContext", ELL_INFORMATION);
}

bool CDummyEGLManager::activateContext(const SExposedVideoData& videoData)
{	 
    Data = videoData;
    return true;
}

const SExposedVideoData& CDummyEGLManager::getContext() const
{
	return Data;
}

bool CDummyEGLManager::swapBuffers()
{
    return true;
}

bool CDummyEGLManager::testEGLError()
{
	return false;
}

}
}

#endif
