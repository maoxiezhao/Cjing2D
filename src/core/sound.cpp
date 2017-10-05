#include"core\sound.h"
#include"core\debug.h"
#include"core\logger.h"
#include"core\fileData.h"
#include"core\oggDecoder.h"
#include"core\music.h"

namespace
{
ALCdevice* device = nullptr;
ALCcontext* context = nullptr;
bool initialized = false;

/**
*	\brief �Զ����ȡ����
*	\param ptr �������ָ��
*	\param size
*	\param nmemb ���ص��ֽ���
*	\param datasource ����Դ
*/
size_t memRead(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	Sound::OggMemory* oggMem = static_cast<Sound::OggMemory*>(datasource);
	const size_t totalSize = oggMem->data.size();

	if (oggMem->position >= totalSize)
	{
		if (oggMem->loop)
		{
			oggMem->position = 0;
		}
		else
		{
			return 0;
		}
	}
	else if (oggMem->position + nmemb >= totalSize)
	{
		nmemb = totalSize - oggMem->position;
	}

	std::memcpy(ptr, oggMem->data.data() + oggMem->position, nmemb);
	oggMem->position += nmemb;

	return nmemb;
}

/**
*	\brief �Զ������ݵ�Ѱַ����
*	\param datasource ����Դ
*	\param offset ����ƫ��
*	\param whence seek�����ͣ���ΪΪSEEK_SET,SEEK_CUR,SEEK_END
*	\return 0��ʾ�ɹ���-1��ʾʧ��
*/
int memSeek(void *datasource, ogg_int64_t offset, int whence)
{
	Sound::OggMemory* oggMem = static_cast<Sound::OggMemory*>(datasource);
	switch (whence)
	{
	case SEEK_SET:
		oggMem->position = (size_t)offset;
		break;
	case SEEK_CUR:
		oggMem->position += (size_t)offset;
		break;
	case SEEK_END:
		oggMem->position = oggMem->data.size() - (size_t)offset;
		break;
	}

	// ������ǰposition
	if (oggMem->position >= oggMem->data.size())
	{
		oggMem->position = oggMem->data.size();
	}
	return 0;
}

/**
*	\brief ���ص�ǰ�Զ������ݵ�λ��
*	\return -1��ʾʧ��,���򷵻ص�ǰλ��
*/
long memTell(void *datasource)
{
	Sound::OggMemory& oggMem = *static_cast<Sound::OggMemory*>(datasource);
	return oggMem.position;
}
}

// �Զ���ṹ�Ļص�����
ov_callbacks Sound::mCallBacks = {
	memRead,
	memSeek,
	nullptr,
	memTell
};

std::map<std::string, Sound> Sound::mSounds;

Sound::Sound() :
	mSoundID(""),
	mBuffer(AL_NONE),
	mSource(AL_NONE)
{
}

Sound::Sound(const std::string soundID) :
	mSoundID(soundID),
	mBuffer(AL_NONE),
	mSource(AL_NONE)
{
}

Sound::~Sound()
{
	if (initialized && mBuffer != AL_NONE)
	{
		for (auto source : mSources)
		{
			alSourceStop(source);
			alSourcei(source, AL_BUFFER, 0);
			alDeleteSources(1, &source);
		}
		alDeleteBuffers(1, &mBuffer);
	}
}

// test function
void Sound::Update(int a)
{
	if (initialized == false)
	{
		return;
	}

	Music::Update();
}

/**
*	\brief ���ص�ǰ��Ч
*/
void Sound::Load()
{
	std::string soundPath = "sounds/" + mSoundID;
	if (!FileData::IsFileExists(soundPath))
	{
		Debug::Error("The sound:" + soundPath + "is not exists.");
	}
	const std::string oggData = FileData::ReadFile(soundPath);

	/** ���ز��� */

}

ALuint Sound::DecodeFile(const std::string file)
{
	return AL_NONE;
}

void Sound::Play()
{
}

Sound * Sound::LoadFile(const std::string soundID)
{
	auto it = mSounds.find(soundID);
	if (it != mSounds.end())
	{
		return &it->second;
	}
	mSounds[soundID] = Sound(soundID);
	return &mSounds[soundID];
}

void Sound::Initialize()
{
	// init openAL
	Logger::Info("Initialize Sound module");
	device = alcOpenDevice(nullptr);
	if (device == nullptr)
	{
		Debug::Error("Failed to initialize alcdevice.");
		return;
	}

	context = alcCreateContext(device, nullptr);
	if (context == nullptr)
	{
		Debug::Error("Failed to create alccontext.");
		alcCloseDevice(device);
		return;
	}

	if (!alcMakeContextCurrent(context))
	{
		Debug::Error("Failed to make sound context.");
		alcDestroyContext(context);
		alcCloseDevice(device);
		return;
	}
	alGenBuffers(0, nullptr);

	// init music
	Music::Initialize();

	initialized = true;
}

void Sound::Update()
{
	Music::Update();
}

void Sound::Quid()
{
	if (initialized)
	{
		Music::Quid();

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
		device = nullptr;
		context = nullptr;

		initialized = false;
	}
}
