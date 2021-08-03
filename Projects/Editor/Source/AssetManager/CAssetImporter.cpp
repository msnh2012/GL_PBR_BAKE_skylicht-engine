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
#include "CAssetImporter.h"

#include <filesystem>
#include <chrono>
#include <sstream>
#include <sys/stat.h>

#include "Utils/CPath.h"
#include "Utils/CStringImp.h"

#if defined(__APPLE_CC__)
namespace fs = std::__fs::filesystem;
#else
namespace fs = std::filesystem;
#endif

namespace Skylicht
{
	namespace Editor
	{
		CAssetImporter::CAssetImporter(std::list<SFileNode*>& listFiles) :
			m_fileID(0),
			m_deleteID(0),
			m_totalDeleted(0)
		{
			m_assetManager = CAssetManager::getInstance();

			m_total = (u32)listFiles.size();
			m_fileIterator = listFiles.begin();
			m_fileIteratorEnd = listFiles.end();

			m_deleteIterator = m_fileDeleted.begin();
			m_deleteIteratorEnd = m_fileDeleted.end();
		}

		CAssetImporter::~CAssetImporter()
		{

		}

		void CAssetImporter::addDeleted(std::list<std::string>& list)
		{
			m_fileDeleted.insert(m_fileDeleted.end(), list.begin(), list.end());

			m_deleteIterator = m_fileDeleted.begin();
			m_deleteIteratorEnd = m_fileDeleted.end();
			m_totalDeleted = (u32)m_fileDeleted.size();
		}

		bool CAssetImporter::loadGUID(int count)
		{
			if (m_fileIterator == m_fileIteratorEnd)
				return true;
			
			for (int j = 0; j < count; j++)
			{
				SFileNode* node = (*m_fileIterator);

				std::string path = node->FullPath;
				
				if (fs::exists(path))
				{
					m_assetManager->readOrGenerateMeta(path.c_str(), node);
				}

				m_lastFile = node->Path;

				++m_fileIterator;
				++m_fileID;

				if (m_fileIterator == m_fileIteratorEnd)
				{
					removeUnusedMeta();
					return true;
				}
			}

			return false;
		}

		void CAssetImporter::getImportStatus(float& percent, std::string& last)
		{
			percent = m_fileID / (float)(m_total);
			last = m_lastFile;
		}

		void CAssetImporter::getDeleteStatus(float& percent, std::string& last)
		{
			percent = m_deleteID / (float)(m_totalDeleted);
			last = m_lastFile;
		}

		bool CAssetImporter::deleteAsset(int count)
		{
			if (m_deleteIterator == m_deleteIteratorEnd)
				return true;

			for (int j = 0; j < count; j++)
			{
				std::string path = (*m_deleteIterator);

				m_assetManager->deleteChildAsset(path.c_str());

				m_lastFile = path;

				++m_deleteIterator;
				++m_deleteID;

				if (m_deleteIterator == m_deleteIteratorEnd)
					return true;
			}

			return false;
		}

		bool CAssetImporter::isFinish()
		{
			if (m_fileIterator == m_fileIteratorEnd && m_deleteIterator == m_deleteIteratorEnd)
				return true;

			return false;
		}

		void CAssetImporter::removeUnusedMeta()
		{
			for (const std::string& path : m_assetManager->m_meta)
			{
				fs::remove(path);
			}

			m_assetManager->m_meta.clear();
		}
	}
}