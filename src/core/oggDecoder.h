#pragma once

#include"core\sound.h"

#include "thirdparty\openAl\al.h"
#include "thirdparty\openAl\alc.h"
#include "thirdparty\ogg\ogg.h"
#include "thirdparty\vorbis\vorbisenc.h"
#include "thirdparty\vorbis\vorbisfile.h"

#include <string>
#include <memory>

/**
*	\brief ogg��ʽ��Ƶ������
*/
class OggDecoder
{
public:
	OggDecoder();

	void Load(const std::string& oggData, bool looped);
	void UnLoad();
	void Decode(ALuint buffer, int nbSamples);

private:
	struct OggVorbisFileDeleter
	{
		void operator()(OggVorbis_File* oggFile)
		{
			ov_clear(oggFile);
			delete oggFile;
		}
	};
	using OggVorbisFileUniquePtr = std::unique_ptr<OggVorbis_File, OggVorbisFileDeleter>;

	ALenum mFormat;
	ALsizei mRate;
	int mChannels;
	bool mIsLoaded;

	OggVorbisFileUniquePtr mVf;			/** oggVoribi File�ṹ*/
	Sound::OggMemory oggMem;			/** ogg�ڴ����ݽṹ */

	ogg_int64_t mStartPcm;
	ogg_int64_t mEndPcm;
};