// This is the main DLL file.

#include "stdafx.h"

#include "LowLevelManagedWrapper.h"
#include "UnmanagedClass.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace VCDiffDotNet {
namespace LowLevel {


	bool Encoder::InitEncoder(array<Byte> ^source, bool checksum, bool interleaved, bool json, bool target_matches)
	{
		vcdiffWrapper = new VCDiffDotNet::VCDiffWrapper();

		vcdiffWrapper->SetEncodingFormatFlags(checksum, interleaved, json, target_matches);

		IntPtr ptrSourceFile(Marshal::AllocHGlobal(source->Length)); 
		try
		{
			Marshal::Copy(source, 0, ptrSourceFile, source->Length);
			vcdiffWrapper->SetSource((const char *)ptrSourceFile.ToPointer(), static_cast<size_t>(source->Length));
		}
		finally
		{
			Marshal::FreeHGlobal(ptrSourceFile);
		}

		return vcdiffWrapper->InitEncoder();
	}

	bool Encoder::StartEncoding([Out] array<Byte> ^%output)
	{
		const char* buff;
		size_t size;

		bool result = vcdiffWrapper->StartEncoding(&buff, &size);
		if(result)
		{
			output = Tools::Convert(buff, size);
			return true;
		}
		return false;
	}

	bool Encoder::EncodeChunk(array<Byte> ^target, [Out] array<Byte> ^%output)
	{
		bool result = false;
		IntPtr ptrTargetFile(Marshal::AllocHGlobal(target->Length)); 
		try
		{
			const char* buff;
			size_t size;
			Marshal::Copy(target, 0, ptrTargetFile, target->Length);
			bool result = vcdiffWrapper->EncodeChunk((const char *)ptrTargetFile.ToPointer(), static_cast<size_t>(target->Length), &buff, &size);
			if(result)
			{
				output = Tools::Convert(buff, size);
			}
		}
		finally
		{
			Marshal::FreeHGlobal(ptrTargetFile);
		}
		return result;
	}

	bool Encoder::FinishEncoding([Out] array<Byte> ^%output)
	{
		const char* buff;
		size_t size;
		bool result = vcdiffWrapper->FinishEncoding(&buff, &size);
		if(result)
		{
			output = Tools::Convert(buff, size);
		}
		return result;
	}

	void Decoder::InitDecoder(array<Byte> ^source, Boolean allow_vcd_target, Int32 max_target_file_size, Int32 max_target_window_size)
	{
		vcdiffWrapper = new VCDiffDotNet::VCDiffWrapper();

		vcdiffWrapper->SetAllowVcdTargetFlag(allow_vcd_target);
		vcdiffWrapper->SetMaxTargetFileSize(max_target_file_size);
		vcdiffWrapper->SetMaxTargetWindowSize(max_target_window_size);

		IntPtr ptrSourceFile(Marshal::AllocHGlobal(source->Length)); 
		try
		{
			Marshal::Copy(source, 0, ptrSourceFile, source->Length);
			vcdiffWrapper->SetSource((const char *)ptrSourceFile.ToPointer(), static_cast<size_t>(source->Length));
		}
		finally
		{
			Marshal::FreeHGlobal(ptrSourceFile);
		}

		vcdiffWrapper->InitDecoder();
	}

	void Decoder::StartDecoding()
	{
		vcdiffWrapper->StartDecoding();
	}

	bool Decoder::DecodeChunk(array<Byte> ^patch, [Out] array<Byte> ^%target)
	{
		bool result = false;
		IntPtr ptrPatchFile(Marshal::AllocHGlobal(patch->Length)); 
		try
		{
			const char* buff;
			size_t size;
			Marshal::Copy(target, 0, ptrPatchFile, patch->Length);
			bool result = vcdiffWrapper->EncodeChunk((const char *)ptrPatchFile.ToPointer(), static_cast<size_t>(patch->Length), &buff, &size);
			if(result)
			{
				target = Tools::Convert(buff, size);
			}
		}
		finally
		{
			Marshal::FreeHGlobal(ptrPatchFile);
		}
		return result;
	}

	bool Decoder::FinishDecoding()
	{
		return vcdiffWrapper->FinishDecoding();
	}

	array<Byte>^ Tools::Convert(const char* buff, size_t size)
	{
		cli::array<Byte>^ bytes = gcnew cli::array<Byte>(size);
		Marshal::Copy(IntPtr((void *)buff), bytes, 0, size);
		delete[] buff;
		return bytes;
	}
}
}