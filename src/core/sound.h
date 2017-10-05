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
*	\brief Sound音频类
*
*	Sound同时用于初始化管理openAL模块，也用于作为音效的实例
*	音效因为数据量小，则直接全部加载到内存中，同时sound类
*	也控制着Music的流程
*/
class Sound
{
public:
	struct OggMemory
	{
		bool loop;
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

	static ov_callbacks mCallBacks;		/** 自定义的ovfile加载时的解析数据行为 */

private:
	std::string mSoundID;
	ALuint mBuffer;
	ALuint mSource;
	std::vector<ALuint> mSources;		/** 管理每个音效的source */

	static std::map<std::string, Sound> mSounds;	/** 管理全部sound实例 */
};