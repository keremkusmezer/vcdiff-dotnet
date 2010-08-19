#include "stdafx.h"

#include <stddef.h>  // size_t

#include <memory>
#include <string>
#include <vector>

#include "google/vcdecoder.h"
#include "google/vcencoder.h"

#include "UnmanagedClass.h"

#pragma unmanaged

namespace vcdiffdotnet {

	class VCDiffWrapper
	{
	private:
		open_vcdiff::VCDiffStreamingEncoder *encoder;
		std::auto_ptr<open_vcdiff::HashedDictionary> hashed_dictionary_;

		open_vcdiff::VCDiffFormatExtensionFlags format_flags;

		bool allow_vcd_target;
		bool target_matches;
		size_t max_target_file_size;
		size_t max_target_window_size;

	public:
		VCDiffWrapper()
		{
			format_flags = open_vcdiff::VCD_STANDARD_FORMAT;
		}

		void Settings(
					bool allow_vcd_target,
					bool checksum,
					bool interleaved,
					bool json,
					bool target_matches,
					size_t max_target_file_size,
					size_t max_target_window_size)
		{
			if (interleaved) {
				format_flags |= open_vcdiff::VCD_FORMAT_INTERLEAVED;
			}
			if (checksum) {
				format_flags |= open_vcdiff::VCD_FORMAT_CHECKSUM;
			}
			if (json) {
				format_flags |= open_vcdiff::VCD_FORMAT_JSON;
			}

			allow_vcd_target = allow_vcd_target;
			target_matches = target_matches;
			max_target_file_size = max_target_file_size;
			max_target_window_size = max_target_window_size;
		};

		bool SetSource(const char* sourceBuff, size_t sourceBuffSize)
		{
			if(sourceBuffSize == 0)
			{
				hashed_dictionary_.reset(new open_vcdiff::HashedDictionary("", 0));
			} 
			else 
			{
				hashed_dictionary_.reset(new open_vcdiff::HashedDictionary(sourceBuff, sourceBuffSize));
			}

			if (!hashed_dictionary_->Init()) 
			{
				//std::cerr << "Error initializing hashed dictionary" << std::endl;
				return false;
			}

			return true;
		};

		bool InitEncoder()
		{
			encoder = new open_vcdiff::VCDiffStreamingEncoder(
							hashed_dictionary_.get(),
							format_flags,
							target_matches);
		};

		bool StartEncoding(const char *buff, size_t *size)
		{
			std::string output;
			bool result = encoder->StartEncoding(&output);
			if(result)
			{
				buff = output.c_str();
				*size = static_cast<size_t>(output.size());
				return true;
			}
			return false;
		};

		bool EncodeChunk(const char* data, size_t len, const char *buff, size_t *size)
		{
			std::string output;
			bool result = encoder->EncodeChunk(data, len, &output);
			if(result)
			{
				buff = output.c_str();
				*size = static_cast<size_t>(output.size());
				return true;
			}
			return false;
		};

		bool FinishEncoding(const char *buff, size_t *size)
		{
			std::string output;
			bool result = encoder->FinishEncoding(&output);
			if(result)
			{
				buff = output.c_str();
				*size = static_cast<size_t>(output.size());
				return true;
			}
			return false;
		};

		bool InitDecoder()
		{
		};


	}; // class VCDiffWrapper

} // namespace vcdiffdotnet

#pragma managed