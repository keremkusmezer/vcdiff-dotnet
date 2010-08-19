// vcdiff-dotnet.h

#pragma once

using namespace System;
using namespace System::IO;

namespace vcdiffdotnet {

	public ref class Setting
	{
	private:
		static const size_t kDefaultMaxTargetSize = 1 << 26;      // 64 MB

		__int64 buffer_size;
		bool allow_vcd_target;
		bool checksum;
		bool interleaved;
		bool json;
		bool target_matches;
		__int64 max_target_file_size;
		__int64 max_target_window_size;

	public:
		Setting()
		{
			buffer_size = 1 << 20;  // 1 MB
			allow_vcd_target = true;
			checksum = false;
			interleaved = false;
			json = false;
			target_matches = false;
			max_target_file_size = kDefaultMaxTargetSize;
			max_target_window_size = kDefaultMaxTargetSize;
		}

		property __int64 BufferSize
		{
			__int64 get() { return buffer_size; }
			void set(__int64 value) { buffer_size = value; }
		}

		property bool AllowVcdTarget
		{
			bool get() { return allow_vcd_target; }
			void set(bool value) { allow_vcd_target = value; }
		}

		property bool Checksum
		{
			bool get() { return checksum; }
			void set(bool value) { checksum = value; }
		}

		property bool Interleaved
		{
			bool get() { return interleaved; }
			void set(bool value) { interleaved = value; }
		}

		property bool Json
		{
			bool get() { return json; }
			void set(bool value) { json = value; }
		}

		property bool TargetMatches
		{
			bool get() { return target_matches; }
			void set(bool value) { target_matches = value; }
		}

		property __int64 MaxTargetFileSize
		{
			__int64 get() { return max_target_file_size; }
			void set(__int64 value) { max_target_file_size = value; }
		}
		
		property __int64 MaxTargetWindowSize
		{
			__int64 get() { return max_target_window_size; }
			void set(__int64 value) { max_target_window_size = value; }
		}
	};

	public ref class VCDiff
	{
	private:
		Setting ^setting;

	public:
		VCDiff();
		VCDiff(Setting ^setting);
		~VCDiff();

		void Encode(System::IO::Stream ^sourceStream,
					System::IO::Stream ^targetStream,
					System::IO::Stream ^patchStream);

		void Patch(System::IO::Stream ^sourceStream,
				   System::IO::Stream ^patchStream,
				   System::IO::Stream ^targetStream);

	private:
		bool WriteOutput(Stream^ stream, cli::array<Byte>^ bytes);
		bool ReadInput(Stream^ targetStream, cli::array<Byte>^ inBuff, size_t *bytes_read);
	};
}
