// This is the main DLL file.

#include "stdafx.h"

#include "vcdiff-dotnet.h"
#include "UnmanagedClass.h"

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace vcdiffdotnet {

	VCDiff::VCDiff()
	{
		this->setting = gcnew Setting();
	}

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
		if(!sourceStream)
			throw gcnew ArgumentNullException(L"sourceStream");
		if(!targetStream)
			throw gcnew ArgumentNullException(L"targetStream");
		if(!patchStream)
			throw gcnew ArgumentNullException(L"patchStream");

		if(!sourceStream->CanRead)
			throw gcnew ArgumentException(L"CanRead == false", L"sourceStream");
		if(!targetStream->CanRead)
			throw gcnew ArgumentException(L"CanRead == false", L"targetStream");
		if(!patchStream->CanWrite)
			throw gcnew ArgumentException(L"CanWrite == false", L"patchStream");

		VCDiffWrapper *vcdiffWrapper = new VCDiffWrapper();
		vcdiffWrapper->Settings(
				this->setting->AllowVcdTarget,
				this->setting->Checksum,
				this->setting->Interleaved,
				this->setting->Json,
				this->setting->TargetMatches,
				this->setting->MaxTargetFileSize,
				this->setting->MaxTargetWindowSize);

		int sourceDataSize = (int)sourceStream->Length;
		array<System::Byte> ^sourceDataBuffer = gcnew array<System::Byte>(sourceDataSize);

		sourceStream->Read(sourceDataBuffer, 0, sourceDataSize);

		IntPtr ptrSourceFile(Marshal::AllocHGlobal(sourceDataSize)); 
		try
		{
			Marshal::Copy(sourceDataBuffer, 0, ptrSourceFile, sourceDataSize);
			vcdiffWrapper->SetSource((const char *)ptrSourceFile.ToPointer(), static_cast<size_t>(sourceDataSize));
		}
		finally
		{
			Marshal::FreeHGlobal(ptrSourceFile);
		}

		if(!vcdiffWrapper->InitEncoder())
			throw gcnew Exception(L"InitEncoder()");
		
		const char* buff;
		size_t size;
		bool result = vcdiffWrapper->StartEncoding(&buff, &size);
		if(!result)
			throw gcnew Exception(L"StartEncoding(...)");
		cli::array<Byte>^ bytes = gcnew cli::array<Byte>(size);
		Marshal::Copy(IntPtr((void *)buff), bytes, 0, size);
		delete[] buff;
		
		cli::array<Byte>^ inBuff = gcnew cli::array<Byte>(this->setting->BufferSize);			
		size_t bytes_read = 0;

		do{
			if (!WriteOutput(patchStream, bytes))
				throw gcnew Exception(L"WriteOutput(...)");
			if (!ReadInput(targetStream, inBuff, &bytes_read))
				throw gcnew Exception(L"ReadInput(...)");

			if(bytes_read > 0)
			{
				bool result;
				IntPtr ptrTargetFile(Marshal::AllocHGlobal(bytes_read)); 
				try
				{
					Marshal::Copy(inBuff, 0, ptrTargetFile, bytes_read);
					
					result = vcdiffWrapper->EncodeChunk(
								(const char*)ptrTargetFile.ToPointer(), 
								static_cast<size_t>(bytes_read),
								&buff, &size);
				}
				finally
				{
					Marshal::FreeHGlobal(ptrTargetFile);
				}

				if(!result)
				{
					throw gcnew Exception(L"EncodeChunk(...)");
				}

				bytes = gcnew cli::array<Byte>(size);
				Marshal::Copy(IntPtr((void *)buff), bytes, 0, size);
				delete[] buff;
			}
		} while (bytes_read != 0);

		if(!vcdiffWrapper->FinishEncoding(&buff, &size))
			throw gcnew Exception(L"FinishEncoding(...)");

		bytes = gcnew cli::array<Byte>(size);
		Marshal::Copy(IntPtr((void *)buff), bytes, 0, size);				
		delete[] buff;

		if(!WriteOutput(patchStream, bytes))
		{
			throw gcnew Exception(L"WriteOutput(...)");
		}
	};

	bool VCDiff::WriteOutput(Stream^ stream, cli::array<Byte>^ bytes)
	{
		stream->Write(bytes, 0, bytes->Length);
		return true;
	}

	bool VCDiff::ReadInput(Stream^ targetStream, cli::array<Byte>^ inBuff, size_t *bytes_read)
	{
		int size = targetStream->Read(inBuff, 0, this->setting->BufferSize);
		*bytes_read = static_cast<size_t>(size);
		return true;
	}

	void VCDiff::Patch(System::IO::Stream ^sourceStream, 
						System::IO::Stream ^patchStream, 
						System::IO::Stream ^targetStream)
	{
		if(!sourceStream)
			throw gcnew ArgumentNullException(L"sourceStream");
		if(!patchStream)
			throw gcnew ArgumentNullException(L"patchStream");
		if(!targetStream)
			throw gcnew ArgumentNullException(L"targetStream");

		if(!sourceStream->CanRead)
			throw gcnew ArgumentException(L"CanRead == false", L"sourceStream");
		if(!patchStream->CanRead)
			throw gcnew ArgumentException(L"CanRead == false", L"patchStream");
		if(!targetStream->CanWrite)
			throw gcnew ArgumentException(L"CanWrite == false", L"targetStream");

		VCDiffWrapper *vcdiffWrapper = new VCDiffWrapper();
		vcdiffWrapper->Settings(
				this->setting->AllowVcdTarget,
				this->setting->Checksum,
				this->setting->Interleaved,
				this->setting->Json,
				this->setting->TargetMatches,
				this->setting->MaxTargetFileSize,
				this->setting->MaxTargetWindowSize);

		int sourceDataSize = (int)sourceStream->Length;
		array<System::Byte> ^sourceDataBuffer = gcnew array<System::Byte>(sourceDataSize);

		sourceStream->Read(sourceDataBuffer, 0, sourceDataSize);

		IntPtr ptrSourceFile(Marshal::AllocHGlobal(sourceDataSize)); 
		try
		{
			Marshal::Copy(sourceDataBuffer, 0, ptrSourceFile, sourceDataSize);
			vcdiffWrapper->SetSource((const char *)ptrSourceFile.ToPointer(), static_cast<size_t>(sourceDataSize));
		}
		finally
		{
			Marshal::FreeHGlobal(ptrSourceFile);
		}

		vcdiffWrapper->InitDecoder();

		cli::array<Byte>^ inBuff = gcnew cli::array<Byte>(this->setting->BufferSize);			
		size_t bytes_read = 0;

		const char* buff;
		size_t size;
		cli::array<Byte>^ bytes;
 
		do 
		{
			if (!ReadInput(patchStream, inBuff, &bytes_read)) 
			{
				throw gcnew Exception(L"ReadInput(...)");
			}
			if (bytes_read > 0) 
			{
				bool result;

				IntPtr ptrPatchFile(Marshal::AllocHGlobal(bytes_read)); 
				try
				{
					Marshal::Copy(inBuff, 0, ptrPatchFile, bytes_read);
					
					result = vcdiffWrapper->DecodeChunk(
								(const char*)ptrPatchFile.ToPointer(), 
								static_cast<size_t>(bytes_read),
								&buff, &size);
				}
				finally
				{
					Marshal::FreeHGlobal(ptrPatchFile);
				}

				if(!result)
				{
					throw gcnew Exception(L"EncodeChunk(...)");
				}

				bytes = gcnew cli::array<Byte>(size);
				Marshal::Copy(IntPtr((void *)buff), bytes, 0, size);		
				delete[] buff;

			}
			if (!WriteOutput(targetStream, bytes)) 
				throw gcnew Exception(L"FinishDecoding()");
		} while (bytes_read != 0);
		
		if (!vcdiffWrapper->FinishDecoding())
			throw gcnew Exception(L"FinishDecoding()");
	};
}

