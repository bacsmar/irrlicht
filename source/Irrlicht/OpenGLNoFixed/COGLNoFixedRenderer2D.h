// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_RENDERER_2D_H_INCLUDED__
#define __C_OGLES2_RENDERER_2D_H_INCLUDED__

#include "IrrCompileConfig.h"

#ifdef _IRR_COMPILE_WITH_OGL_NOFIXED_

#include "COGLNoFixedMaterialRenderer.h"

namespace irr
{
	namespace video
	{
		class COGLNoFixedRenderer2D : public COGLNoFixedMaterialRenderer
		{
		public:
			COGLNoFixedRenderer2D(const c8* vertexShaderProgram, const c8* pixelShaderProgram, COpenGLNoFixedDriver* driver);
			~COGLNoFixedRenderer2D();

			void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
			                   bool resetAllRenderstates, IMaterialRendererServices* services) override;

			bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype) override;

		protected:
			s32 ThicknessID;
			s32 TextureUsageID;
		};
	}
}

#endif
#endif

