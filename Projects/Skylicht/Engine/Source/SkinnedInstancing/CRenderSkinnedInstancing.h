/*
!@
MIT License

Copyright (c) 2024 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once



#include "Entity/CEntityPrefab.h"
#include "Entity/CEntityHandler.h"
#include "Material/CMaterial.h"
#include "Material/CMaterialManager.h"
#include "RenderMesh/CRenderMeshData.h"
#include "Transform/CWorldTransformData.h"
#include "CTransformTextureData.h"
#include "Animation/CAnimationClip.h"

namespace Skylicht
{
	class SKYLICHT_API CRenderSkinnedInstancing : public CEntityHandler
	{
	protected:
		CEntity* m_root;
		core::array<CEntity*> m_baseEntities;

		std::vector<CWorldTransformData*> m_renderTransforms;
		std::vector<CWorldTransformData*> m_transforms;

		std::vector<CRenderMeshData*> m_renderers;
		std::vector<CTransformTextureData*> m_textures;

		ArrayMaterial m_materials;

		std::string m_meshFile;
		std::string m_materialFile;

		bool m_loadTexcoord2;
		bool m_loadNormal;
		bool m_fixInverseNormal;

		IVertexBuffer* m_instancingTransform;
		IVertexBuffer* m_instancingLighting;
		int m_shareData;

	public:
		CRenderSkinnedInstancing();

		virtual ~CRenderSkinnedInstancing();

		virtual void initComponent();

		virtual void updateComponent();

		virtual CObjectSerializable* createSerializable();

		virtual void loadSerializable(CObjectSerializable* object);

		virtual CEntity* spawn();

		static bool setAnimation(CEntity* entity, int animTextureIndex, CAnimationClip* clipInfo, float currentTime = 0.0f, int bakeFps = 60, bool loop = true, bool pause = false);

		static bool setAnimation(CEntity* entity, int animTextureIndex, CAnimationClip* clipInfo, float clipBegin, float clipDuration, float currentTime = 0.0f, int bakeFps = 60, bool loop = true, bool pause = false);

	public:

		void initTextureTransform(core::matrix4* transforms, u32 w, u32 h, std::map<std::string, int>& bones);

		void refreshModelAndMaterial();

		void initFromPrefab(CEntityPrefab* prefab);

		void initFromMeshFile(const char* path);

		void initMaterialFromFile(const char* material);

		void initMaterial(ArrayMaterial& materials, bool cloneMaterial = false);

		void applyShareInstancingBuffer();

		inline int getMaterialCount()
		{
			return (int)m_materials.size();
		}

		CMaterial* getMaterial(int i)
		{
			return m_materials[i];
		}

		core::array<CEntity*>& getEntities()
		{
			return m_baseEntities;
		}

		std::vector<CRenderMeshData*>& getRenderers()
		{
			return m_renderers;
		}

		std::vector<CWorldTransformData*>& getRenderTransforms()
		{
			return m_renderTransforms;
		}

		std::vector<CWorldTransformData*>& getAllTransforms()
		{
			return m_transforms;
		}

		DECLARE_GETTYPENAME(CRenderSkinnedInstancing);

	protected:

		void releaseMaterial();

		void releaseEntities();

		void releaseBaseEntities();
	};
}