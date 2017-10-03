#pragma once

#include"common\common.h"

#include"thirdparty\openAl\al.h"
#include"thirdparty\openAl\alc.h"
#include"thirdparty\ogg\ogg.h"
#include"thirdparty\vorbis\vorbisenc.h"
#include"thirdparty\vorbis\vorbisfile.h"

#include<map>

class OggDecoder;

/**
*	\brief “Ù∆µ¿‡
*/
class Sound
{
public:
	struct OggMemory
	{
		size_t position;
		string data;
	};

	Sound();
	Sound(const std::string soundID);
	~Sound();

	void Update(int a);
	void Load();
	ALuint DecodeFile(const std::string file);
	void Play();

	// static function
	static Sound* LoadFile(const std::string soundID);
	static void Initialize();
	static void Update();
	static void Quid();

	static ov_callbacks mCallBacks;
	static std::unique_ptr<OggDecoder> mOggDecoder;

private:
	std::string mSoundID;
	ALuint mBuffer;
	ALuint mSource;
	std::vector<ALuint> mSources;

	static constexpr int numBuffers = 8;
	ALuint mBuffers[numBuffers];
};