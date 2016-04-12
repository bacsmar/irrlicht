// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OGLES2_SL_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OGLES2_SL_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGL_NOFIXED_

#include "EMaterialTypes.h"
#include "IMaterialRenderer.h"
#include "IMaterialRendererServices.h"
#include "IGPUProgrammingServices.h"
#include "irrArray.h"
#include "irrString.h"

#include "COGLNoFixedCommon.h"

namespace irr
{
namespace video  
{

class COpenGLNoFixedDriver;

class COGLNoFixedMaterialRenderer : public IMaterialRenderer, public IMaterialRendererServices
{
public:

	COGLNoFixedMaterialRenderer(
		COpenGLNoFixedDriver* driver,
		s32& outMaterialTypeNr, 
		const c8* vertexShaderProgram = 0,
		const c8* pixelShaderProgram = 0,
		IShaderConstantSetCallBack* callback = 0,
		E_MATERIAL_TYPE baseMaterial = EMT_SOLID,
		s32 userData = 0);

	virtual ~COGLNoFixedMaterialRenderer();

	GLuint getProgram() const;

	void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services) _IRR_OVERRIDE_;

	bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype) _IRR_OVERRIDE_;

	void OnUnsetMaterial() _IRR_OVERRIDE_;

	bool isTransparent() const _IRR_OVERRIDE_;

	s32 getRenderCapability() const _IRR_OVERRIDE_;

	virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates) _IRR_OVERRIDE_;

	virtual s32 getVertexShaderAttribID(const c8* name);
	virtual s32 getVertexShaderConstantID(const c8* name) _IRR_OVERRIDE_;
	virtual s32 getPixelShaderConstantID(const c8* name) _IRR_OVERRIDE_;
	virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1) _IRR_OVERRIDE_;
	virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1) _IRR_OVERRIDE_;
	virtual bool setVertexShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
	virtual bool setVertexShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;
	virtual bool setPixelShaderConstant(s32 index, const f32* floats, int count) _IRR_OVERRIDE_;
	virtual bool setPixelShaderConstant(s32 index, const s32* ints, int count) _IRR_OVERRIDE_;	

	IVideoDriver* getVideoDriver() override;

protected:

	explicit COGLNoFixedMaterialRenderer(COpenGLNoFixedDriver* driver,
					IShaderConstantSetCallBack* callback = 0,
					E_MATERIAL_TYPE baseMaterial = EMT_SOLID,
					s32 userData = 0);

	void init(s32& outMaterialTypeNr, const c8* vertexShaderProgram, const c8* pixelShaderProgram, bool addMaterial = true);

	bool createShader(GLenum shaderType, const char* shader);
	bool linkProgram();
	
	COpenGLNoFixedDriver* Driver;
	IShaderConstantSetCallBack* CallBack;

	bool Alpha;
	bool Blending;
	bool FixedBlending;

	struct SUniformInfo
	{
		core::stringc name;
		GLenum type;
		GLint location;
	};

	struct SAtrribInfo
	{
		core::stringc name;
		GLenum type;
		GLint location;
	};

	GLuint Program;
	core::array<SUniformInfo> UniformInfo;
	core::array<SAtrribInfo> AttribInfo;
	s32 UserData;
};


}
}

#endif
#endif

