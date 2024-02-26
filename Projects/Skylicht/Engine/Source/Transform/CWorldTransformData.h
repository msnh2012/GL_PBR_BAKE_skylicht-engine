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

#pragma once

#include "Entity/IEntityData.h"

namespace Skylicht
{
	class CWorldTransformData : public IEntityData
	{
	public:
		bool IsWorldTransform;
		bool HasChanged;
		bool NeedValidate;
		core::matrix4 World;
		core::matrix4 Relative;
		int Depth;
		int ParentIndex;
		int AttachParentIndex;
		std::string Name;
		CWorldTransformData* Parent;

		DECLARE_DATA_TYPE_INDEX;

	public:
		CWorldTransformData();

		virtual ~CWorldTransformData();

		virtual bool serializable(CMemoryStream* stream);

		virtual bool deserializable(CMemoryStream* stream);

		DECLARE_GETTYPENAME(CWorldTransformData);
	};
}