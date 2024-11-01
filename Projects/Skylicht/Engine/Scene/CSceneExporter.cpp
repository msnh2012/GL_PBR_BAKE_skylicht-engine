/*
!@
MIT License

Copyright (c) 2021 Skylicht Technology CO., LTD

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
#include "CSceneExporter.h"
#include "Utils/CPath.h"

namespace Skylicht
{
	void CSceneExporter::loadChildObjectSerializable(CContainerObject* container, CObjectSerializable* data)
	{
		CObjectSerializable* childs = new CObjectSerializable("Childs");
		data->addProperty(childs);
		data->autoRelease(childs);

		ArrayGameObject* go = container->getChilds();
		for (size_t i = 0, n = go->size(); i < n; i++)
		{
			CGameObject* childObject = go->at(i);

			if (childObject->isEditorObject())
				continue;

			CObjectSerializable* childData = childObject->createSerializable();
			childs->addProperty(childData);
			childs->autoRelease(childData);

			CContainerObject* childContainer = dynamic_cast<CContainerObject*>(childObject);
			if (childContainer != NULL)
				loadChildObjectSerializable(childContainer, childData);
		}
	}

	CObjectSerializable* CSceneExporter::exportGameObject(CGameObject* object)
	{
		CObjectSerializable* data = object->createSerializable();

		CContainerObject* container = dynamic_cast<CContainerObject*>(object);
		if (container != NULL)
			loadChildObjectSerializable(container, data);

		return data;
	}

	void CSceneExporter::exportGameObject(CGameObject* object, const char* path)
	{
		CObjectSerializable* data = object->createSerializable();

		CContainerObject* container = dynamic_cast<CContainerObject*>(object);
		if (container != NULL)
			loadChildObjectSerializable(container, data);

		data->save(path);
		delete data;
	}

	void CSceneExporter::exportScene(CScene* scene, const char* path)
	{
		CObjectSerializable* data = scene->createSerializable();

		ArrayZone* zone = scene->getAllZone();
		ArrayZoneIter i = zone->begin(), end = zone->end();

		std::stack<CObjectSerializable*> stack;

		while (i != end)
		{
			CZone* zone = (*i);

			if (zone->isEditorObject())
			{
				++i;
				continue;
			}

			CObjectSerializable* zoneData = zone->createSerializable();
			loadChildObjectSerializable(zone, zoneData);

			data->addProperty(zoneData);
			data->autoRelease(zoneData);
			++i;

			stack.push(zoneData);
		}

		data->save(path);

		// list all components, that used
		std::map<std::string, int> usedComponents;
		while (stack.size() > 0)
		{
			CObjectSerializable* obj = stack.top();
			stack.pop();

			CObjectSerializable* components = obj->getProperty<CObjectSerializable>("Components");
			if (components)
			{
				for (u32 i = 0, n = components->getNumProperty(); i < n; i++)
				{
					CObjectSerializable* comp = dynamic_cast<CObjectSerializable*>(components->getPropertyID(i));
					if (comp)
					{
						usedComponents[comp->Name] = 1;
					}
				}
			}

			CObjectSerializable* childs = obj->getProperty<CObjectSerializable>("Childs");
			if (childs)
			{
				for (u32 i = 0, n = childs->getNumProperty(); i < n; i++)
				{
					CObjectSerializable* childObj = dynamic_cast<CObjectSerializable*>(childs->getPropertyID(i));
					if (childObj)
						stack.push(childObj);
				}
			}
		}

		std::string file = CPath::replaceFileExt(std::string(path), std::string(".txt"));

		io::IWriteFile* writeFile = getIrrlichtDevice()->getFileSystem()->createAndWriteFile(file.c_str());

		std::string text;
		text = "// File generated by function: CSceneExporter::exportScene\n";
		writeFile->write(text.c_str(), (u32)text.size());

		for (auto it : usedComponents)
		{
			text = "USE_COMPONENT(";
			text += it.first;
			text += ");\n";
			writeFile->write(text.c_str(), (u32)text.size());
		}
		writeFile->drop();

		delete data;
	}
}