#include "core\oggDecoder.h"
#include "core\sound.h"
#include "core\debug.h"

#include<vector>

OggDecoder::OggDecoder():
	mFormat(AL_NONE),
	mRate(0),
	mIsLoaded(false),
	mVf(nullptr),
	mStartPcm(-1),
	mEndPcm(-1)
{
}

void OggDecoder::Load(const std::string& oggData)
{
	// 加载oggFile
	mVf = OggVorbisFileUniquePtr(new OggVorbis_File());

	oggMem.position = 0;
	oggMem.data = oggData;

	int error = ov_open_callbacks(&oggMem, mVf.get(), nullptr, 0, Sound::mCallBacks);
	if (error != 0)
	{
		Debug::Error("Failed to open ogg file.");
		return;
	}

	// 获取ogg相关信息
	vorbis_info* oggInfo = ov_info(mVf.get(), -1);
	mChannels = oggInfo->channels;
	if (oggInfo->channels == 1)
	{
		mFormat = AL_FORMAT_MONO16;
	}
	else if (oggInfo->channels == 2)
	{
		mFormat = AL_FORMAT_STEREO16;
	}
	mRate = ALsizei(oggInfo->rate);

	// loop info
	//vorbis_comment* header = ov_comment(mVf.get(), -1);
	//if (header == nullptr)
	//{
	//	return;
	//}
	//const char* loopStartString = vorbis_comment_query(header, "LOOPSTART", 0);
	//const char* loopEndString = vorbis_comment_query(header, "LOOPEND", 0);
	//const char* loopLengthString = vorbis_comment_query(header, "LOOPLENGTH", 0);


	mIsLoaded = true;
}

/**
*	\brief 解码ogg数据
*/
void OggDecoder::Decode(ALuint buffer, int nbSamples) 
{
	if (!mIsLoaded)
	{
		return;
	}

	// 解析sampler数据
	int bitstream = 0;
	long remainReadBytes = nbSamples * mChannels * sizeof(ALshort);

	std::vector<ALshort> samplesData(nbSamples * mChannels);
	long totalReadBytes = 0;
	long curReadByte = 0;
	do
	{
		long maxReadBytes = remainReadBytes;
		ogg_int64_t currentPcm = ov_pcm_tell(mVf.get());
		// 因为解码的数据是段落式，需要知道当前是否
		// 到了pcm_end

		curReadByte = ov_read(
			mVf.get(), (
			(char*)samplesData.data()) + totalReadBytes, 
			maxReadBytes, 
			0,
			2, 
			1, 
			&bitstream
		);

		if (curReadByte < 0)
		{
			Debug::Error("Error Decoding ogg.");
			return;
		}
		else
		{
			remainReadBytes -= curReadByte;
			totalReadBytes += curReadByte;
		}
	}
	while (remainReadBytes > 0 && curReadByte > 0);

	// 生成al buffer
	alBufferData(buffer, mFormat, samplesData.data(), ALsizei(totalReadBytes), mRate);
	if (alGetError() != AL_NO_ERROR)
	{
		Debug::Error("Failed to fill audio buffer with decode ogg data.");
	}
}