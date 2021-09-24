/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the Rights to use, copy, modify,
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
#include "Utils/CActivator.h"
#include "Lighting/CDirectionalLight.h"
#include "Components/CDependentComponent.h"
#include "CGizmosDirectionLight.h"

#include "GameObject/CGameObject.h"
#include "Editor/CEditor.h"
#include "SpriteDraw/CSprite.h"
#include "Handles/CHandles.h"

namespace Skylicht
{
	namespace Editor
	{
		ACTIVATOR_REGISTER(CGizmosDirectionLight);

		DEPENDENT_COMPONENT_REGISTER(CDirectionalLight, CGizmosDirectionLight);

		CGizmosDirectionLight::CGizmosDirectionLight() :
			m_directionLight(NULL)
		{

		}

		CGizmosDirectionLight::~CGizmosDirectionLight()
		{

		}

		void CGizmosDirectionLight::initComponent()
		{
			CSprite* spriteDraw = m_gameObject->addComponent<CSprite>();
			spriteDraw->setFrame(CEditor::getInstance()->getSpriteIcon()->getFrame("light"), 1.0f, SColor(255, 255, 255, 255));
			spriteDraw->setCenter(true);
			spriteDraw->setBillboard(true);
			spriteDraw->setAutoScaleInViewSpace(true);
			spriteDraw->setEnableSerializable(false);

			addLinkComponent(spriteDraw);

			m_directionLight = m_gameObject->getComponent<CDirectionalLight>();
		}

		void CGizmosDirectionLight::updateComponent()
		{
			const core::vector3df pos = m_gameObject->getPosition();
			const core::vector3df v = m_directionLight->getDirection() * 2.0f;

			CHandles::getInstance()->drawLine(pos, pos + v, SColor(255, 255, 255, 255));
		}
	}
}