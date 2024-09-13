/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

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

#include "CGroup.h"

#include "Systems/CParticleSystem.h"

namespace Skylicht
{
	namespace Particle
	{
		CGroup::CGroup() :
			m_renderer(NULL),
			Gravity(0.0f, 0.0f, 0.0f),
			Friction(0.0f),
			LifeMin(1.0f),
			LifeMax(2.0f),
			OrientationNormal(1.0f, 0.0f, 0.0f),
			OrientationUp(0.0f, 1.0f, 0.0f)
		{
			m_particleSystem = new CParticleSystem();

			m_instancingSystem = new CParticleInstancingSystem();
			m_cpuBufferSystem = new CParticleCPUBufferSystem();

			m_instancing = new CParticleInstancing();
			m_cpuBuffer = new CParticleCPUBuffer();
		}

		CGroup::~CGroup()
		{
			for (IParticleCallback* cb : m_callback)
				cb->OnGroupDestroy();

			for (CModel* m : m_models)
				delete m;
			m_models.clear();

			for (CInterpolator* i : m_interpolators)
				delete i;
			m_interpolators.clear();

			delete m_particleSystem;

			delete m_instancingSystem;
			delete m_cpuBufferSystem;

			delete m_instancing;
			delete m_cpuBuffer;
		}

		IRenderer* CGroup::setRenderer(IRenderer* r)
		{
			m_renderer = r;
			if (m_renderer->useInstancing() == true)
				m_renderer->getParticleBuffer(m_instancing->getMeshBuffer());
			else
				m_renderer->getParticleBuffer(m_cpuBuffer->getMeshBuffer());
			return r;
		}

		core::vector3df CGroup::getTransformPosition(const core::vector3df& pos)
		{
			core::vector3df p = pos;
			m_world.transformVect(p);
			return p;
		}

		core::vector3df CGroup::getTransformVector(const core::vector3df& vec)
		{
			core::vector3df p = vec;
			m_world.rotateVect(p);
			return p;
		}

		void CGroup::update(bool visible)
		{
			float dt = getTimeStep();

			updateLaunchEmitter();

			CParticle* particles = m_particles.pointer();
			u32 numParticles = m_particles.size();

			if (visible == true)
			{
				// update particle system
				m_particleSystem->update(particles, numParticles, this, dt);

				for (ISystem* s : m_systems)
				{
					if (s->isEnable() == true)
						s->update(particles, numParticles, this, dt);
				}
			}
			else
			{
				// we just update life time of hide particle
				m_particleSystem->updateLifeTime(particles, numParticles, this, dt);
			}

			for (IParticleCallback* cb : m_callback)
				cb->OnParticleUpdate(particles, numParticles, this, dt);

			if (numParticles > 0)
				m_bbox.reset(particles[0].Position);

			// remove die particle & update box
			for (u32 i = 0; i < numParticles; i++)
			{
				CParticle& p = particles[i];

				if (p.Life < 0)
				{
					// remove dead particle
					remove(i);
					--i;
					--numParticles;
				}
				else
				{
					// add bbox
					m_bbox.addInternalPoint(p.Position);
				}
			}

			// update instancing buffer		
			if (visible == true && m_renderer != NULL)
			{
				if (m_renderer->useInstancing() == true)
					m_instancingSystem->update(particles, numParticles, this, dt);
				else
					m_cpuBufferSystem->update(particles, numParticles, this, dt);
			}

			bornParticle();
		}

		void CGroup::updateLaunchEmitter()
		{
			float dt = getTimeStep();

			// update emitter
			m_launch.set_used(0);
			for (CEmitter* e : m_emitters)
			{
				u32 nb = e->updateNumber(dt);
				if (nb > 0)
				{
					SLaunchParticle data;
					data.Emitter = e;
					data.Number = nb;
					m_launch.push_back(data);
				}
			}
		}

		void CGroup::bornParticle()
		{
			u32 emiterId = 0;
			u32 emiterLaunch = m_launch.size();

			// born new particle
			for (u32 i = emiterId; i < emiterLaunch; i++)
			{
				SLaunchParticle& launch = m_launch[i];
				if (launch.Number > 0)
				{
					CParticle* newParticles = create(launch.Number);
					for (u32 j = 0, n = launch.Number; j < n; j++)
					{
						CParticle& p = newParticles[j];
						launchParticle(p, launch);
					}
				}
			}
		}

		bool CGroup::launchParticle(CParticle& p, SLaunchParticle& launch)
		{
			initParticleLifeTime(p);

			if (p.LifeTime > 0)
			{
				launch.Emitter->emitParticle(p, launch.Emitter->getZone(), this);
				launch.Number--;

				initParticleModel(p);
			}

			return launch.Number == 0;
		}

		void CGroup::initParticleModel(CParticle& p)
		{
			for (CModel* m : m_models)
			{
				EParticleParams t = m->getType();

				if (m->haveStart() == true)
					p.StartValue[t] = m->getRandomStart();
				else
					p.StartValue[t] = 0.0f;

				if (m->haveEnd() == true)
					p.EndValue[t] = m->getRandomEnd();
				else
					p.EndValue[t] = p.StartValue[t];

				if (t == Particle::RotateSpeedX ||
					t == Particle::RotateSpeedY ||
					t == Particle::RotateSpeedZ)
				{
					p.HaveRotate = true;
				}
				else if (t == Particle::RotateX)
					p.Rotation.X = p.StartValue[t];
				else if (t == Particle::RotateY)
					p.Rotation.Y = p.StartValue[t];
				else if (t == Particle::RotateZ)
					p.Rotation.Z = p.StartValue[t];
			}

			for (IParticleCallback* cb : m_callback)
				cb->OnParticleBorn(p);
		}

		int CGroup::addParticleByEmitter(CEmitter* emitter, const core::vector3df& position, const core::vector3df& subEmitterDirection)
		{
			CParticle* p = create(1);

			initParticleLifeTime(*p);

			if (p->LifeTime > 0)
			{
				emitter->generateVelocity(*p, emitter->getZone(), this);

				initParticleModel(*p);

				p->Position = position;
				p->SubEmitterDirection = subEmitterDirection;
			}

			return (int)p->Index;
		}

		int CGroup::addParticleVelocityByEmitter(CEmitter* emitter, const core::vector3df& position, const core::vector3df& velocity)
		{
			CParticle* p = create(1);

			initParticleLifeTime(*p);

			if (p->LifeTime > 0)
			{
				emitter->generateVelocity(*p, emitter->getZone(), this);

				initParticleModel(*p);

				p->Position = position;
				p->Velocity = velocity;
			}

			return (int)p->Index;
		}

		CParticle* CGroup::create(u32 num)
		{
			u32 total = m_particles.size();
			for (u32 i = 0; i < num; i++)
			{
				m_particles.push_back(CParticle(total + i));
			}
			return m_particles.pointer() + total;
		}

		void CGroup::remove(u32 index)
		{
			u32 total = m_particles.size();
			if (index >= total)
				return;

			if (index != total - 1)
			{
				for (IParticleCallback* cb : m_callback)
					cb->OnSwapParticleData(m_particles[index], m_particles.getLast());

				m_particles[index].swap(m_particles.getLast());
			}

			for (IParticleCallback* cb : m_callback)
				cb->OnParticleDead(m_particles.getLast());

			m_particles.set_used(total - 1);
		}

		CModel* CGroup::createModel(EParticleParams param)
		{
			CModel* m = getModel(param);

			if (m == NULL)
			{
				m = new CModel(param);
				m_models.push_back(m);
			}

			return m;
		}

		CModel* CGroup::getModel(EParticleParams param)
		{
			for (CModel* m : m_models)
			{
				if (m->getType() == param)
					return m;
			}

			return NULL;
		}

		void CGroup::deleteModel(EParticleParams param)
		{
			int id = -1;

			for (u32 i = 0, n = (u32)m_models.size(); i < n; i++)
			{
				if (m_models[i]->getType() == param)
				{
					id = i;
					break;
				}
			}

			if (id >= 0)
				m_models.erase(m_models.begin() + id);
		}

		CInterpolator* CGroup::createInterpolator()
		{
			CInterpolator* interpolator = new CInterpolator();
			m_interpolators.push_back(interpolator);
			return interpolator;
		}

		void CGroup::deleteInterpolator(CInterpolator* interpolator)
		{
			int id = -1;

			for (u32 i = 0, n = (u32)m_interpolators.size(); i < n; i++)
			{
				if (m_interpolators[i] == interpolator)
				{
					id = i;
					break;
				}
			}

			if (id >= 0)
				m_interpolators.erase(m_interpolators.begin() + id);
		}
	}
}