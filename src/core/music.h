#pragma once

#include"common\common.h"
#include"core\oggDecoder.h"
#include"core\sound.h"

/**
*	\brief Music音频类
*
*	Music用于播放长时间的音频文件，Music同一时间
*	只能播放一个Music,且对于music的数据处理是分段式
*	加载置内存，并动态释放。
*
*	2017.10.5 目前只支持ogg的解码，近期内添加MP3解码
*/
class Music
{
public:
	/**
	*	音频格式枚举值
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

	static constexpr int numBuffers  = 8;    /** 加载到内存中的音频数据缓存片段个数 */
	static constexpr long numSamples = 16384;/** 每个数据片段的采样数 */
	ALuint mSource;
	ALuint mBuffers[numBuffers];			 /** 加载到内存中的音频数据缓存 */
};

inline const string& Music::GetMusicID()const
{
	return mMusicID;
}

inline int Music::GetVolumen() const
{
	return mVolumn;
}
