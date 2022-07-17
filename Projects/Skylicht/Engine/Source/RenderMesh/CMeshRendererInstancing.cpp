/*
!@
MIT License

Copyright (c) 2019 Skylicht Technology CO., LTD

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

#include "pch.h"
#include "CMeshRendererInstancing.h"

#include "Culling/CCullingData.h"
#include "Entity/CEntityManager.h"

#include "Material/Shader/ShaderCallback/CShaderSH.h"
#include "Material/Shader/ShaderCallback/CShaderLighting.h"
#include "Material/Shader/ShaderCallback/CShaderMaterial.h"

namespace Skylicht
{
	CMeshRendererInstancing::CMeshRendererInstancing()
	{
		m_pipelineType = IRenderPipeline::Mix;
	}

	CMeshRendererInstancing::~CMeshRendererInstancing()
	{
		for (auto it : m_groups)
		{
			delete it.second;
		}
	}

	void CMeshRendererInstancing::beginQuery(CEntityManager* entityManager)
	{
		m_meshs.set_used(0);

		for (auto it : m_groups)
		{
			SMeshInstancingGroup* group = it.second;
			group->IndirectLightings.set_used(0);
			group->Materials.set_used(0);
			group->Transforms.set_used(0);
		}
	}

	void CMeshRendererInstancing::onQuery(CEntityManager* entityManager, CEntity** entities, int numEntity)
	{
		for (int i = 0; i < numEntity; i++)
		{
			CEntity* entity = entities[i];

			CRenderMeshData* meshData = GET_ENTITY_DATA(entity, CRenderMeshData);
			if (meshData != NULL)
			{
				if (meshData->getMesh() == NULL ||
					meshData->getInstancingData() == NULL)
					continue;

				// do not render gpu skinning, pass for CSkinMeshRenderer
				if (meshData->isSkinnedMesh() == true &&
					meshData->isSoftwareSkinning() == false)
					continue;

				if (meshData->isInstancing())
				{
					bool cullingVisible = true;

					// get culling result from CCullingSystem
					CCullingData* cullingData = GET_ENTITY_DATA(entity, CCullingData);
					if (cullingData != NULL)
						cullingVisible = cullingData->Visible;

					// only render visible culling mesh
					if (cullingVisible == true)
					{
						m_meshs.push_back(meshData);
					}
				}
			}
		}
	}

	void CMeshRendererInstancing::init(CEntityManager* entityManager)
	{

	}

	void CMeshRendererInstancing::update(CEntityManager* entityManager)
	{
		// need sort render by material, texture, mesh		
		u32 numEntity = m_meshs.size();
		CRenderMeshData** renderData = m_meshs.pointer();

		// update instancing
		for (u32 i = 0; i < numEntity; i++)
		{
			SMeshInstancingData* data = renderData[i]->getInstancingData();

			SMeshInstancingGroup* group = m_groups[data];
			if (group == NULL)
			{
				group = new SMeshInstancingGroup();
				m_groups[data] = group;
			}

			CEntity* entity = entityManager->getEntity(renderData[i]->EntityIndex);

			CWorldTransformData* transform = GET_ENTITY_DATA(entity, CWorldTransformData);
			CIndirectLightingData* indirect = GET_ENTITY_DATA(entity, CIndirectLightingData);

			group->Transforms.push_back(transform);
			group->IndirectLightings.push_back(indirect);
		}

		// bake instancing in group
		for (auto it : m_groups)
		{
			SMeshInstancingData* data = it.first;
			SMeshInstancingGroup* group = it.second;

			u32 count = group->Transforms.size();
			if (count == 0)
				continue;

			for (u32 i = 0, n = data->RenderMeshBuffers.size(); i < n; i++)
			{
				group->Materials.set_used(0);

				for (u32 j = 0; j < count; j++)
					group->Materials.push_back(data->Materials[i]);

				// batching transform & material data to buffer
				data->Instancing[i]->batchIntancing(
					data->InstancingBuffer[i],
					group->Materials,
					group->Transforms,
					group->IndirectLightings
				);
			}
		}
	}

	void CMeshRendererInstancing::render(CEntityManager* entityManager)
	{
		IVideoDriver* driver = getVideoDriver();
		IRenderPipeline* rp = entityManager->getRenderPipeline();

		driver->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		for (auto& it : m_groups)
		{
			SMeshInstancingData* data = it.first;
			SMeshInstancingGroup* group = it.second;

			u32 count = group->Transforms.size();
			if (count == 0)
				continue;

			u32 numMeshBuffer = data->RenderMeshBuffers.size();
			for (u32 i = 0; i < numMeshBuffer; i++)
			{
				CShader* shader = data->Materials[i]->getShader();

				if (!rp->canRenderShader(shader))
					continue;

				CShaderMaterial::setMaterial(NULL);

				int materialType = shader->getInstancingShader()->getMaterialRenderID();

				rp->drawInstancingMeshBuffer(
					(CMesh*)data->InstancingMesh,
					i,
					materialType,
					entityManager,
					false
				);
			}
		}
	}
}