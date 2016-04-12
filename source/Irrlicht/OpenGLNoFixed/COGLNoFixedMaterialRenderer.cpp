// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COGLNoFixedMaterialRenderer.h"

#ifdef _IRR_COMPILE_WITH_OGL_NOFIXED_
#include "EVertexAttributes.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IVideoDriver.h"
#include "../os.h"

#include "COpenGLNoFixedDriver.h"

#include "../COpenGLCoreCacheHandler.h"

namespace irr
{
	namespace video
	{
		COGLNoFixedMaterialRenderer::COGLNoFixedMaterialRenderer(COpenGLNoFixedDriver* driver,
		                                                         s32& outMaterialTypeNr,
		                                                         const c8* vertexShaderProgram,
		                                                         const c8* pixelShaderProgram,
		                                                         IShaderConstantSetCallBack* callback,
		                                                         E_MATERIAL_TYPE baseMaterial,
		                                                         s32 userData)
			: Driver(driver), CallBack(callback), Alpha(false), Blending(false), FixedBlending(false), Program(0), UserData(userData)
		{
#ifdef _DEBUG
			setDebugName("COGLNoFixedMaterialRenderer");
#endif

			switch (baseMaterial)
			{
			case EMT_TRANSPARENT_VERTEX_ALPHA:
			case EMT_TRANSPARENT_ALPHA_CHANNEL:
			case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
			case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
				Alpha = true;
				break;
			case EMT_TRANSPARENT_ADD_COLOR:
			case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
			case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
				FixedBlending = true;
				break;
			case EMT_ONETEXTURE_BLEND:
				Blending = true;
				break;
			default:
				break;
			}

			if (CallBack)
				CallBack->grab();

			init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram);
		}


		COGLNoFixedMaterialRenderer::COGLNoFixedMaterialRenderer(COpenGLNoFixedDriver* driver,
		                                                         IShaderConstantSetCallBack* callback,
		                                                         E_MATERIAL_TYPE baseMaterial, s32 userData)
			: Driver(driver), CallBack(callback), Alpha(false), Blending(false), FixedBlending(false), Program(0), UserData(userData)
		{
			switch (baseMaterial)
			{
			case EMT_TRANSPARENT_VERTEX_ALPHA:
			case EMT_TRANSPARENT_ALPHA_CHANNEL:
			case EMT_NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA:
			case EMT_PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA:
				Alpha = true;
				break;
			case EMT_TRANSPARENT_ADD_COLOR:
			case EMT_NORMAL_MAP_TRANSPARENT_ADD_COLOR:
			case EMT_PARALLAX_MAP_TRANSPARENT_ADD_COLOR:
				FixedBlending = true;
				break;
			case EMT_ONETEXTURE_BLEND:
				Blending = true;
				break;
			default:
				break;
			}

			if (CallBack)
				CallBack->grab();
		}


		COGLNoFixedMaterialRenderer::~COGLNoFixedMaterialRenderer()
		{
			if (CallBack)
				CallBack->drop();

			if (Program)
			{
				GLuint shaders[8];
				GLint count;
				Driver->extGlGetAttachedShaders(Program, 8, &count, shaders);

				count = core::min_(count, 8);
				for (GLint i = 0; i < count; ++i)
					Driver->extGlDeleteShader(shaders[i]);
				Driver->extGlDeleteProgram(Program);
				Program = 0;
			}

			UniformInfo.clear();
		}

		GLuint COGLNoFixedMaterialRenderer::getProgram() const
		{
			return Program;
		}

		void COGLNoFixedMaterialRenderer::init(s32& outMaterialTypeNr,
		                                       const c8* vertexShaderProgram,
		                                       const c8* pixelShaderProgram,
		                                       bool addMaterial)
		{
			outMaterialTypeNr = -1;

			Program = Driver->extGlCreateProgram();

			if (!Program)
				return;

			if (vertexShaderProgram)
				if (!createShader(GL_VERTEX_SHADER, vertexShaderProgram))
					return;

			if (pixelShaderProgram)
				if (!createShader(GL_FRAGMENT_SHADER, pixelShaderProgram))
					return;

			for (size_t i = 0; i < EVA_COUNT; ++i)
				Driver->extGlBindAttribLocation(Program, i, sBuiltInVertexAttributeNames[i]);

			if (!linkProgram())
				return;

			if (addMaterial)
				outMaterialTypeNr = Driver->addMaterialRenderer(this);
		}


		bool COGLNoFixedMaterialRenderer::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
		{
			if (CallBack && Program)
				CallBack->OnSetConstants(this, UserData);

			return true;
		}


		void COGLNoFixedMaterialRenderer::OnSetMaterial(const video::SMaterial& material,
		                                                const video::SMaterial& lastMaterial,
		                                                bool resetAllRenderstates,
		                                                video::IMaterialRendererServices* services)
		{
			auto cacheHandler = Driver->getCacheHandler();

			cacheHandler->setProgram(Program);

			Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

			if (Alpha)
			{
				cacheHandler->setBlend(true);
				cacheHandler->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else if (FixedBlending)
			{
				cacheHandler->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
				cacheHandler->setBlend(true);
			}
			else if (Blending)
			{
				E_BLEND_FACTOR srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact;
				E_MODULATE_FUNC modulate;
				u32 alphaSource;
				unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulate, alphaSource, material.MaterialTypeParam);

				cacheHandler->setBlendFuncSeparate(Driver->getGLBlend(srcRGBFact), Driver->getGLBlend(dstRGBFact),
				                                   Driver->getGLBlend(srcAlphaFact), Driver->getGLBlend(dstAlphaFact));

				cacheHandler->setBlend(true);
			}

			if (CallBack)
				CallBack->OnSetMaterial(material);
		}


		void COGLNoFixedMaterialRenderer::OnUnsetMaterial()
		{
		}


		bool COGLNoFixedMaterialRenderer::isTransparent() const
		{
			return (Alpha || Blending || FixedBlending);
		}


		s32 COGLNoFixedMaterialRenderer::getRenderCapability() const
		{
			return 0;
		}


		bool COGLNoFixedMaterialRenderer::createShader(GLenum shaderType, const char* shader)
		{
			if (Program)
			{
				GLuint shaderHandle = Driver->extGlCreateShader(shaderType);
				Driver->extGlShaderSource(shaderHandle, 1, &shader, nullptr);
				Driver->extGlCompileShader(shaderHandle);

				GLint status = 0;

				Driver->extGlGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

				if (status != GL_TRUE)
				{
					os::Printer::log("GLSL shader failed to compile", ELL_ERROR);

					GLint maxLength = 0;
					GLint length;

					Driver->extGlGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH,
					                         &maxLength);

					if (maxLength)
					{
						GLchar* infoLog = new GLchar[maxLength];
						Driver->extGlGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
						os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
						delete [] infoLog;
					}

					return false;
				}

				Driver->extGlAttachShader(Program, shaderHandle);
			}

			return true;
		}


		bool COGLNoFixedMaterialRenderer::linkProgram()
		{
			if (Program)
			{
				Driver->extGlLinkProgram(Program);

				GLint status = 0;

				Driver->extGlGetProgramiv(Program, GL_LINK_STATUS, &status);

				if (!status)
				{
					os::Printer::log("GLSL shader program failed to link", ELL_ERROR);

					GLint maxLength = 0;
					GLsizei length;

					Driver->extGlGetProgramiv(Program, GL_INFO_LOG_LENGTH, &maxLength);

					if (maxLength)
					{
						GLchar* infoLog = new GLchar[maxLength];
						Driver->extGlGetProgramInfoLog(Program, maxLength, &length, infoLog);
						os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
						delete [] infoLog;
					}

					return false;
				}

				GLint num = 0;

				Driver->extGlGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num);

				if (num == 0)
					return true;

				GLint maxlen = 0;

				Driver->extGlGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

				if (maxlen == 0)
				{
					os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
					return false;
				}

				// seems that some implementations use an extra null terminator.
				++maxlen;
				c8* buf = new c8[maxlen];

				UniformInfo.clear();
				UniformInfo.reallocate(num);

				for (GLint i = 0; i < num; ++i)
				{
					SUniformInfo ui;
					memset(buf, 0, maxlen);

					GLint size;
					Driver->extGlGetActiveUniform(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLchar*>(buf));

					core::stringc name = "";

					// array support, workaround for some bugged drivers.
					for (s32 i = 0; i < maxlen; ++i)
					{
						if (buf[i] == '[' || buf[i] == '\0')
							break;

						name += buf[i];
					}

					ui.name = name;
					ui.location = Driver->extGlGetUniformLocation(Program, buf);

					UniformInfo.push_back(ui);
				}

				delete [] buf;

				// get vertex attrib info
				// http://stackoverflow.com/questions/440144/in-opengl-is-there-a-way-to-get-a-list-of-all-uniforms-attribs-used-by-a-shade
				GLint numAttrib = 0;
				Driver->extGlGetProgramiv(Program, GL_ACTIVE_ATTRIBUTES, &numAttrib);
				GLint attribMaxlen = 0;
				Driver->extGlGetProgramiv(Program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attribMaxlen);

				if (attribMaxlen == 0)
				{
					os::Printer::log("GLSL: failed to retrieve vertex attrib information", ELL_ERROR);
					return false;
				}

				// seems that some implementations use an extra null terminator
				{
					++attribMaxlen;
					c8* buf = new c8[attribMaxlen];

					AttribInfo.clear();
					AttribInfo.reallocate(numAttrib);

					for (GLint i = 0; i < numAttrib; ++i)
					{
						SAtrribInfo info;
						memset(buf, 0, attribMaxlen);

						GLint size;
						Driver->extGlGetActiveAttrib(Program, i, attribMaxlen, nullptr, &size, &info.type, reinterpret_cast<GLchar*>(buf));

						core::stringc name = "";

						// array support, workaround for some bugged drivers.
						for (s32 i = 0; i < maxlen; ++i)
						{
							if (buf[i] == '[' || buf[i] == '\0')
								break;

							name += buf[i];
						}

						info.name = name;
						info.location = Driver->extGlGetAttribLocation(Program, buf);

						AttribInfo.push_back(info);
					}

					delete[] buf;
				}
			}

			return true;
		}


		void COGLNoFixedMaterialRenderer::setBasicRenderStates(const SMaterial& material,
		                                                       const SMaterial& lastMaterial,
		                                                       bool resetAllRenderstates)
		{
			Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
		}

		s32 COGLNoFixedMaterialRenderer::getVertexShaderAttribID(const c8* name)
		{
			for (u32 i = 0; i < AttribInfo.size(); ++i)
			{
				if (AttribInfo[i].name == name)
					return i;
			}

			return -1;
		}

		s32 COGLNoFixedMaterialRenderer::getVertexShaderConstantID(const c8* name)
		{
			return getPixelShaderConstantID(name);
		}

		s32 COGLNoFixedMaterialRenderer::getPixelShaderConstantID(const c8* name)
		{
			for (u32 i = 0; i < UniformInfo.size(); ++i)
			{
				if (UniformInfo[i].name == name)
					return i;
			}

			return -1;
		}

		void COGLNoFixedMaterialRenderer::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
		{
			os::Printer::log("Cannot set constant, please use high level shader call instead.", ELL_WARNING);
		}

		void COGLNoFixedMaterialRenderer::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
		{
			os::Printer::log("Cannot set constant, use high level shader call.", ELL_WARNING);
		}

		bool COGLNoFixedMaterialRenderer::setVertexShaderConstant(s32 index, const f32* floats, int count)
		{
			return setPixelShaderConstant(index, floats, count);
		}

		bool COGLNoFixedMaterialRenderer::setVertexShaderConstant(s32 index, const s32* ints, int count)
		{
			return setPixelShaderConstant(index, ints, count);
		}

		bool COGLNoFixedMaterialRenderer::setPixelShaderConstant(s32 index, const f32* floats, int count)
		{
			if (index < 0 || UniformInfo[index].location < 0)
				return false;

			bool status = true;

			switch (UniformInfo[index].type)
			{
			case GL_FLOAT:
				Driver->extGlUniform1fv(UniformInfo[index].location, count, floats);
				break;
			case GL_FLOAT_VEC2:
				Driver->extGlUniform2fv(UniformInfo[index].location, count / 2, floats);
				break;
			case GL_FLOAT_VEC3:
				Driver->extGlUniform3fv(UniformInfo[index].location, count / 3, floats);
				break;
			case GL_FLOAT_VEC4:
				Driver->extGlUniform4fv(UniformInfo[index].location, count / 4, floats);
				break;
			case GL_FLOAT_MAT2:
				Driver->extGlUniformMatrix2fv(UniformInfo[index].location, count / 4, false, floats);
				break;
			case GL_FLOAT_MAT3:
				Driver->extGlUniformMatrix3fv(UniformInfo[index].location, count / 9, false, floats);
				break;
			case GL_FLOAT_MAT4:
				Driver->extGlUniformMatrix4fv(UniformInfo[index].location, count / 16, false, floats);
				break;
			case GL_SAMPLER_2D:
			case GL_SAMPLER_CUBE:
				{
					if (floats)
					{
						const GLint id = (GLint)(*floats);
						Driver->extGlUniform1iv(UniformInfo[index].location, 1, &id);
					}
					else
						status = false;
				}
				break;
			default:
				status = false;
				break;
			}

			return status;
		}

		bool COGLNoFixedMaterialRenderer::setPixelShaderConstant(s32 index, const s32* ints, int count)
		{
			if (index < 0 || UniformInfo[index].location < 0)
				return false;

			bool status = true;

			switch (UniformInfo[index].type)
			{
			case GL_INT:
			case GL_BOOL:
				Driver->extGlUniform1iv(UniformInfo[index].location, count, ints);
				break;
			case GL_INT_VEC2:
			case GL_BOOL_VEC2:
				Driver->extGlUniform2iv(UniformInfo[index].location, count / 2, ints);
				break;
			case GL_INT_VEC3:
			case GL_BOOL_VEC3:
				Driver->extGlUniform3iv(UniformInfo[index].location, count / 3, ints);
				break;
			case GL_INT_VEC4:
			case GL_BOOL_VEC4:
				Driver->extGlUniform4iv(UniformInfo[index].location, count / 4, ints);
				break;
			case GL_SAMPLER_2D:
			case GL_SAMPLER_CUBE:
				Driver->extGlUniform1iv(UniformInfo[index].location, 1, ints);
				break;
			default:
				status = false;
				break;
			}

			return status;
		}

		IVideoDriver* COGLNoFixedMaterialRenderer::getVideoDriver()
		{
			return Driver;
		}
	}
}


#endif

