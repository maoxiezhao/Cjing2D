#pragma once

#include"common\common.h"
#include"core\oggDecoder.h"
#include"core\sound.h"

/**
*	\brief Music��Ƶ��
*
*	Music���ڲ��ų�ʱ�����Ƶ�ļ���Musicͬһʱ��
*	ֻ�ܲ���һ��Music,�Ҷ���music�����ݴ����Ƿֶ�ʽ
*	�������ڴ棬����̬�ͷš�
*
*	2017.10.5 Ŀǰֻ֧��ogg�Ľ��룬���������MP3����
*/
class Music
{
public:
	/**
	*	��Ƶ��ʽö��ֵ
	*/
	enum Format
	{
		NONE,
		OGG,
		MP3
	};

public:
	Music(const std::string& musicID, bool loop);

	bool UpdatePlay();
	bool Play();
	void Stop();
	Format ParseFormat(const std::string& filename)const;

	/** Status */
	const string& GetMusicID()const;
	void SetVolumn(int volumn);
	int GetVolumen()const;

	/** Static function */
	static void Initialize();
	static void Update();
	static void Quid();
	static void PlayMusic(const std::string& musicID, bool loop);
	static void StopMusic();
	static void SetMusicVolume(int volume);
	static string GetCurMusicID();

private:
	std::string mMusicID;
	bool mLoop;
	Format mFormat;
	int mVolumn;

	static std::unique_ptr<Music> mCurMusic;
	static std::unique_ptr<OggDecoder> mOggDecoder;

	static constexpr int numBuffers  = 8;    /** ���ص��ڴ��е���Ƶ���ݻ���Ƭ�θ��� */
	static constexpr long numSamples = 16384;/** ÿ������Ƭ�εĲ����� */
	ALuint mSource;
	ALuint mBuffers[numBuffers];			 /** ���ص��ڴ��е���Ƶ���ݻ��� */
};

inline const string& Music::GetMusicID()const
{
	return mMusicID;
}

inline int Music::GetVolumen() const
{
	return mVolumn;
}
