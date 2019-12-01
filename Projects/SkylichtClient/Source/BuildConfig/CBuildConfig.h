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

#ifndef _CBUILD_CONFIG_H_
#define _CBUILD_CONFIG_H_

#include "pch.h"

namespace Skylicht
{

	class CBuildConfig
	{
	public:
		static std::string	SaveFolder;
		static std::string	ProfileFolder;
		static std::string	MountFolder;
		static std::string	DataFolder;

		static std::string  DeviceID;
		static std::string  OSVersion;
		static std::string  Factory;
		static std::string  Model;

		static std::string	CPID;

		static std::string	APKPath;
		static std::string	ObbPath;

		static std::string	BundleID;

		static std::string	DLCVersion;

		static bool UseZipData;
	};

}

#endif