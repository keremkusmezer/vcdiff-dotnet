// LowLevelManagedWrapper.h

#pragma once

#include "UnmanagedClass.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace VCDiffDotNet {
namespace LowLevel {

	public ref class Encoder
	{
	private:
		VCDiffDotNet::VCDiffWrapper *vcdiffWrapper;

	public:
		bool InitEncoder(array<Byte> ^source, bool checksum, bool interleaved, bool json, bool target_matches);
		bool StartEncoding([Out] array<Byte> ^%output);
		bool EncodeChunk(array<Byte> ^target, [Out] array<Byte> ^%output);
		bool FinishEncoding([Out] array<Byte> ^%output);

	public:
		~Encoder() // IDisposable
		{
			// release resource
			vcdiffWrapper = NULL;
		}

	protected:
		!Encoder() // Finalize
		{
			// release resource
		}
	};

	public ref class Decoder
	{
	private:
		VCDiffDotNet::VCDiffWrapper *vcdiffWrapper;

	public:
		void InitDecoder(array<Byte> ^source, bool allow_vcd_target, int max_target_file_size, int max_target_window_size);
		void StartDecoding();
		bool DecodeChunk(array<Byte> ^patch, [Out] array<Byte> ^%target);
		bool FinishDecoding();

	public:
		~Decoder() // IDisposable
		{
			// release resource
			vcdiffWrapper = NULL;
		}

	protected:
		!Decoder() // Finalize
		{
			// release resource
		}
	};

	private ref class Tools sealed
	{
	public:
		static array<Byte>^ Convert(const char* buff, size_t size);
	};
}
}