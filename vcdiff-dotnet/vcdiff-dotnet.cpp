// This is the main DLL file.

#include "stdafx.h"

#include "vcdiff-dotnet.h"

namespace vcdiffdotnet {

	VCDiff::VCDiff(Setting ^setting)
	{
		this->setting = setting;
	};

	VCDiff::~VCDiff()
	{
	};

	void VCDiff::Encode(System::IO::Stream ^sourceStream, 
						System::IO::Stream ^targetStream, 
						System::IO::Stream ^patchStream)
	{

	};

	void VCDiff::Patch(System::IO::Stream ^sourceStream, 
						System::IO::Stream ^patchStream, 
						System::IO::Stream ^targetStream)
	{
	};

}

