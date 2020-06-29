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

#include "IEntityData.h"

namespace Skylicht
{
	class CEntityManager;
	class CEntityPrefab;

	class CEntity
	{
		friend class CEntityManager;
		friend class CEntityPrefab;

	protected:
		bool m_visible;
		bool m_alive;
		int m_index;

		core::array<IEntityData*> m_data;

	public:
		CEntity(CEntityManager *mgr);
		CEntity(CEntityPrefab *mgr);

		virtual ~CEntity();

		template<class T>
		T* addData();

		IEntityData* addDataByActivator(const char *dataType);

		inline int getDataCount()
		{
			return (int)m_data.size();
		}

		IEntityData* getData(int dataIndex);

		template<class T>
		T* getData();

		template<class T>
		bool removeData();

		void removeAllData();

		inline int getIndex()
		{
			return m_index;
		}

		inline bool isAlive()
		{
			return m_alive;
		}

		inline bool isVisible()
		{
			return m_visible;
		}

		inline void setVisible(bool b)
		{
			m_visible = b;
		}

	protected:

		inline void setAlive(bool b)
		{
			m_alive = b;
		}

	};

	template<class T>
	T* CEntity::addData()
	{
		T *newData = new T();
		IEntityData *data = dynamic_cast<IEntityData*>(newData);
		if (data == NULL)
		{
			char exceptionInfo[512];
			sprintf(exceptionInfo, "CEntity::addData %s must inherit IEntityData", typeid(T).name());
			os::Printer::log(exceptionInfo);

			delete newData;
			return NULL;
		}

		// also save this entity index
		data->EntityIndex = m_index;

		// add to list data
		m_data.push_back(newData);
		return newData;
	}

	template<class T>
	T* CEntity::getData()
	{
		IEntityData** data = m_data.pointer();
		u32 numData = m_data.size();

		for (u32 i = 0; i < numData; i++)
		{
			T* t = dynamic_cast<T*>(data[i]);
			if (t)
			{
				return t;
			}
		}

		return NULL;
	}

	template<class T>
	bool CEntity::removeData()
	{
		IEntityData** data = m_data.pointer();
		u32 numData = m_data.size();

		for (u32 i = 0; i < numData; i++)
		{
			if (typeid(T) == typeid(*data[i]))
			{
				delete data[i];
				m_data.erase(i);
				return true;
			}
		}

		return false;
	}
}