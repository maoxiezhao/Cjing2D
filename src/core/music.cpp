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
*	\brief ���µ�ǰ����״̬
*	\return true ���ż�����false���Ž���
*/
bool Music::UpdatePlay()
{
	/**
	*	����Ƿ�����Ѿ����Ž�����buffer
	*	���������decode�µ�����
	*/
	ALint nbEmpty;
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &nbEmpty);
	
	for (int i = 0; i < nbEmpty; i++)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mSource, 1, &buffer);
		// ������ʽ
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
*	\brief ���ŵ�ǰmusic
*
*	���ݴ�ʱ�������ڴ���ö�Ӧdecoder�������󶨵�AL����
*/
bool Music::Play()
{
	if (!isInitialized)
	{
		return false;
	}

	// ����openAL��buffer��source
	alGenBuffers(numBuffers, mBuffers);
	alGenSources(1, &mSource);
	alSourcef(mSource, AL_GAIN, 1.0f);	

	// ���ؽ�������
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
	// ������ʽ
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

	// ����
	alSourceQueueBuffers(mSource, numBuffers, mBuffers);
	alSourcePlay(mSource);

	return true;
}

void Music::Stop()
{
	alSourceStop(mSource);

	// �ͷŻ����
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

	// �ͷ�decoder��Դ
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
*	\brief ���õ�ǰ����
*	\param volumn ������С��0-100
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
*	\brief ������ǰ�ļ�������Ƶ��ʽ
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
*	\brief ȫ��Music����
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
*	\brief ����ָ��ID��music
*
*	�����ǰ���ڲ���music�����ֹͣ�󲥷���music
*/
void Music::PlayMusic(const std::string & musicID, bool loop)
{
	if (musicID != "" && musicID != GetCurMusicID())
	{
		// �����ǰmusic���ڣ�����ֹͣ
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
*	\brief ���ص�ǰmusic id����������ڣ��򷵻ء���
*/
string Music::GetCurMusicID()
{
	if (mCurMusic != nullptr)
	{
		return mCurMusic->GetMusicID();
	}
	return string("");
}
