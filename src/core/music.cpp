#include "music.h"
#include "core\debug.h"
#include "core\fileData.h"

namespace{

bool isInitialized = false;
}

std::unique_ptr<Music> Music::mCurMusic = nullptr;
std::unique_ptr<OggDecoder> Music::mOggDecoder = nullptr;
constexpr int Music::numBuffers;
constexpr long Music::numSamples;

Music::Music(const std::string & musicID, bool loop):
	mMusicID(musicID),
	mLoop(loop),
	mFormat(Format::NONE),
	mSource(AL_NONE)
{
	for (int i = 0; i < numBuffers; i++)
	{
		mBuffers[i] = AL_NONE;
	}
}

/**
*	\brief 更新当前播放状态
*	\return true 播放继续，false播放结束
*/
bool Music::UpdatePlay()
{
	/**
	*	检测是否存在已经播放结束的buffer
	*	如果存在则decode新的数据
	*/
	ALint nbEmpty;
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &nbEmpty);
	
	for (int i = 0; i < nbEmpty; i++)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mSource, 1, &buffer);
		// 解析格式
		mOggDecoder->Decode(buffer, numSamples);

		alSourceQueueBuffers(mSource, 1, &buffer);
	}	

	ALint status;
	alGetSourcei(mSource, AL_SOURCE_STATE, &status);
	if (status != AL_PLAYING)
	{
		alSourcePlay(mSource);
	}

	alGetSourcei(mSource, AL_SOURCE_STATE, &status);
	return status == AL_PLAYING;
}

/**
*	\brief 播放当前music
*
*	数据此时被读入内存调用对应decoder解析，绑定到AL缓存
*/
bool Music::Play()
{
	if (!isInitialized)
	{
		return false;
	}

	// 创建openAL的buffer和source
	alGenBuffers(numBuffers, mBuffers);
	alGenSources(1, &mSource);
	alSourcef(mSource, AL_GAIN, 1.0f);	

	// 加载解析数据
	std::string soundPath = "sounds/" + mMusicID;
	if (!FileData::IsFileExists(soundPath))
	{
		Debug::Warning("The sound:" + soundPath + "is not exists.");
		// clear
		alDeleteSources(1, &mSource);
		alDeleteBuffers(numBuffers, mBuffers);
		for (int i = 0; i < numBuffers; i++)
		{
			mBuffers[i] = AL_NONE;
		}
		return false;
	}
	std::string musicBuffer = FileData::ReadFile(soundPath);
	mFormat = ParseFormat(mMusicID);
	// 解析格式
	switch (mFormat)
	{
	case Music::NONE:
		return false;
		break;
	case Music::OGG:
		mOggDecoder->Load(musicBuffer, mLoop);
		for (int i = 0; i < numBuffers; i++)
		{
			mOggDecoder->Decode(mBuffers[i], 16384);
		}
		break;
	case Music::MP3:
		mOggDecoder->Load(musicBuffer, mLoop);
		for (int i = 0; i < numBuffers; i++)
		{
			mOggDecoder->Decode(mBuffers[i], numSamples);
		}
		break;
	}

	// 播放
	alSourceQueueBuffers(mSource, numBuffers, mBuffers);
	alSourcePlay(mSource);

	return true;
}

void Music::Stop()
{
	alSourceStop(mSource);

	// 释放缓存段
	ALint numQueues;
	ALuint buffer;
	alGetSourcei(mSource, AL_BUFFERS_QUEUED, &numQueues);
	for (int i = 0; i < numQueues; i++)
	{
		alSourceUnqueueBuffers(mSource, 1, &buffer);
	}
	alSourcei(mSource, AL_BUFFER, 0);

	// clear
	alDeleteSources(1, &mSource);
	alDeleteBuffers(numBuffers, mBuffers);
	for (int i = 0; i < numBuffers; i++)
	{
		mBuffers[i] = AL_NONE;
	}

	// 释放decoder资源
	switch (mFormat)
	{
	case Music::NONE:
		break;
	case Music::OGG:
		mOggDecoder->UnLoad();
		break;
	case Music::MP3:
		mOggDecoder->UnLoad();
		break;
	}
}

/**
*	\brief 设置当前音量
*	\param volumn 音量大小在0-100
*/
void Music::SetVolumn(int volumn)
{
	if (!isInitialized || mSource == AL_NONE)
	{
		return;
	}
	Debug::CheckAssertion(volumn >= 0 && volumn <= 100,
		"The volum must between 0 and 100.");

	alSourcef(mSource, AL_GAIN, ALfloat(volumn / 100.0f));
}

/**
*	\brief 解析当前文件名的音频格式
*/
Music::Format Music::ParseFormat(const std::string & filename) const
{
	int dotPos = filename.find(".");
	if (dotPos == string::npos)
	{
		return Format::NONE;
	}
	const string formatStr = filename.substr(dotPos + 1);
	Format result = Format::NONE;
	if (formatStr == "ogg")
	{
		result = Format::OGG;
	}
	else if(formatStr == "mp3")
	{
		result = Format::MP3;
	}
	return result;
}

void Music::Initialize()
{
	mOggDecoder = std::unique_ptr<OggDecoder>(new OggDecoder());

	isInitialized = true;
}

/**
*	\brief 全局Music更新
*/
void Music::Update()
{
	if (!isInitialized )
	{
		return;
	}
	if (mCurMusic != nullptr)
	{
		bool playFinished = mCurMusic->UpdatePlay();
		if (playFinished == false)
		{
			mCurMusic->Stop();
			mCurMusic = nullptr;
		}
	}
}

void Music::Quid()
{
	if (isInitialized)
	{
		if (mCurMusic != nullptr)
		{
			mCurMusic->Stop();
			mCurMusic = nullptr;
		}
		mOggDecoder = nullptr;
	}
}

/**
*	\brief 播放指定ID的music
*
*	如果当前存在播放music，则会停止后播放新music
*/
void Music::PlayMusic(const std::string & musicID, bool loop)
{
	if (musicID != "" && musicID != GetCurMusicID())
	{
		// 如果当前music存在，则先停止
		if (mCurMusic != nullptr)
		{
			mCurMusic->Stop();
			mCurMusic = nullptr;
		}

		mCurMusic = std::unique_ptr<Music>(new Music(musicID, loop));
		if (!mCurMusic->Play())
		{
			Debug::Warning("Failed to play the music:" + musicID);
			mCurMusic = nullptr;
		}
	}
}

void Music::StopMusic()
{
	if (mCurMusic != nullptr)
	{
		mCurMusic->Stop();
		mCurMusic = nullptr;
	}
}

void Music::SetMusicVolume(int volume)
{
	if (mCurMusic != nullptr)
		mCurMusic->SetVolumn(volume);
}

/**
*	\brief 返回当前music id，如果不存在，则返回“”
*/
string Music::GetCurMusicID()
{
	if (mCurMusic != nullptr)
	{
		return mCurMusic->GetMusicID();
	}
	return string("");
}
