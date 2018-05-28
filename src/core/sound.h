#pragma once

#include"common\common.h"

#include"thirdparty\openAl\al.h"
#include"thirdparty\openAl\alc.h"
#include"thirdparty\ogg\ogg.h"
#include"thirdparty\vorbis\vorbisenc.h"
#include"thirdparty\vorbis\vorbisfile.h"

#include<map>
#include<list>

class OggDecoder;

/**
*	\brief Sound��Ƶ��
*
*	Soundͬʱ���ڳ�ʼ������openALģ�飬Ҳ������Ϊ��Ч��ʵ��
*	��Ч��Ϊ������С����ֱ��ȫ�����ص��ڴ��У�ͬʱsound��
*	Ҳ������Music������
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

	bool UpdatePlay();
	void Load();
	ALuint DecodeFile(const std::string& filePath);
	void Play();

	// static function
	static Sound* LoadFile(const std::string soundID);
	static void Initialize();
	static void Update();
	static void Quit();
	static void PlaySound(const std::string& id);

	static ov_callbacks mCallBacks;					/** �Զ����ovfile����ʱ�Ľ���������Ϊ */

private:
	std::string mSoundID;
	ALuint mBuffer;
	ALfloat mVolume;
	std::list<ALuint> mSources;					/** ����ÿ����Ч��source */

	static std::list<Sound*> mCurrentSounds;		/** ��ǰ���ڲ��ŵ�sounds */
	static std::map<std::string, Sound> mAllSounds;	/** ����ȫ��soundʵ�� */
};