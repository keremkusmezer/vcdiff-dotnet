#include "stdafx.h"

#pragma unmanaged
#include <stddef.h>  // size_t

#include <memory>
#include <string>
#include <vector>

#include "google/vcdecoder.h"
#include "google/vcencoder.h"
#pragma managed

#include "UnmanagedClass.h"

#pragma unmanaged
namespace vcdiffdotnet {
	
	VCDiffWrapper::VCDiffWrapper()
	{
		format_flags = open_vcdiff::VCD_STANDARD_FORMAT;
	}

	void VCDiffWrapper::Settings(
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

	void VCDiffWrapper::SetSource(const char* sourceBuff, size_t sourceBuffSize)
	{
		if(sourceBuffSize != 0)
		{
			sourceDataBuffer.resize(sourceBuffSize);
			memcpy(&sourceDataBuffer[0], sourceBuff, sourceBuffSize);
		}
	};

	bool VCDiffWrapper::InitEncoder()
	{
		if (sourceDataBuffer.empty())
		{
			hashed_dictionary_.reset(new open_vcdiff::HashedDictionary("", 0));
		} 
		else 
		{
			hashed_dictionary_.reset(
				new open_vcdiff::HashedDictionary(
					&sourceDataBuffer[0], 
					sourceDataBuffer.size()));
		}

		if (!hashed_dictionary_->Init()) 
		{
			//std::cerr << "Error initializing hashed dictionary" << std::endl;
			return false;
		}

		encoder = new open_vcdiff::VCDiffStreamingEncoder(
						hashed_dictionary_.get(),
						format_flags,
						target_matches);
		return true;
	};

	bool VCDiffWrapper::StartEncoding(const char **buff, size_t *size)
	{
		std::string output;
		bool result = encoder->StartEncoding(&output);
		if(result)
		{
			*buff = output.c_str();
			*size = static_cast<size_t>(output.size());
			return true;
		}
		return false;
	};

	bool VCDiffWrapper::EncodeChunk(const char* data, size_t len, const char **buff, size_t *size)
	{
		std::string output;
		bool result = encoder->EncodeChunk(data, len, &output);
		if(result)
		{
			*buff = output.c_str();
			*size = static_cast<size_t>(output.size());
			return true;
		}
		return false;
	};

	bool VCDiffWrapper::FinishEncoding(const char **buff, size_t *size)
	{
		std::string output;
		bool result = encoder->FinishEncoding(&output);
		if(result)
		{
			*buff = output.c_str();
			*size = static_cast<size_t>(output.size());
			return true;
		}
		return false;
	};

	void VCDiffWrapper::InitDecoder()
	{
		decoder = new open_vcdiff::VCDiffStreamingDecoder();

		decoder->SetMaximumTargetFileSize(max_target_file_size);
		decoder->SetMaximumTargetWindowSize(max_target_window_size);
		decoder->SetAllowVcdTarget(allow_vcd_target);
	};

	void VCDiffWrapper::StartDecoding()
	{
		// Issue 6: Visual Studio STL produces a runtime exception
		// if &dictionary_[0] is attempted for an empty dictionary.
		if (sourceDataBuffer.empty()) 
		{
			decoder->StartDecoding("", 0);
		} 
		else 
		{
			decoder->StartDecoding(&sourceDataBuffer[0], sourceDataBuffer.size());
		}
	}

	bool VCDiffWrapper::DecodeChunk(const char* data, size_t len, const char **buff, size_t *size)
	{
		std::string output;
		bool result = decoder->DecodeChunk(data, len, &output);
		if(result)
		{
			*buff = output.c_str();
			*size = static_cast<size_t>(output.size());
			return true;
		}
		return false;
	};

	bool VCDiffWrapper::FinishDecoding()
	{
		return decoder->FinishDecoding();
	}
	

} // namespace vcdiffdotnet

#pragma managed