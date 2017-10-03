#include"core\sound.h"
#include"core\debug.h"
#include"core\logger.h"
#include"core\fileData.h"
#include"core\oggDecoder.h"

namespace
{
ALCdevice* device = nullptr;
ALCcontext* context = nullptr;
bool initialized = false;

/**
*	\brief 自定义读取函数
*	\param ptr 数据输出指针
*	\param size
*	\param nmemb 加载的字节数
*	\param datasource 数据源
*/
size_t memRead(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	Sound::OggMemory* oggMem = static_cast<Sound::OggMemory*>(datasource);
	const size_t totalSize = oggMem->data.size();

	if (oggMem->position >= totalSize)
	{
		return 0;
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
*	\brief 自定义数据的寻址函数
*	\param datasource 数据源
*	\param offset 数据偏移
*	\param whence seek的类型，分为为SEEK_SET,SEEK_CUR,SEEK_END
*	\return 0表示成功，-1表示失败
*/
int memSeek(void *datasource, ogg_int64_t offset, int whence)
{
	Sound::OggMemory* oggMem = static_cast<Sound::OggMemory*>(datasource);
	switch (whence)
	{
	case SEEK_SET:
		oggMem->position = offset;
		break;
	case SEEK_CUR:
		oggMem->position += offset;
		break;
	case SEEK_END:
		oggMem->position = oggMem->data.size() - offset;
		break;
	}

	// 修正当前position
	if (oggMem->position >= oggMem->data.size())
	{
		oggMem->position = oggMem->data.size();
	}
	return 0;
}

/**
*	\brief 返回当前自定义数据的位置
*	\return -1表示失败,否则返回当前位置
*/
long memTell(void *datasource)
{
	Sound::OggMemory& oggMem = *static_cast<Sound::OggMemory*>(datasource);
	return oggMem.position;
}
}

// 自定义结构的回掉函数
ov_callbacks Sound::mCallBacks = {
	memRead,
	memSeek,
	nullptr,
	memTell
};

std::unique_ptr<OggDecoder> Sound::mOggDecoder = nullptr;

constexpr int Sound::numBuffers;

Sound::Sound() :
	Sound(std::string("")) 
{
	for (int i = 0; i < numBuffers; i++)
	{
		mBuffers[i] = AL_NONE;
	}
}

Sound::Sound(const std::string soundID) :
	mSoundID(soundID),
	mBuffer(AL_NONE),
	mSource(AL_NONE)
{
	for (int i = 0; i < numBuffers; i++)
	{
		mBuffers[i] = AL_NONE;
	}
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
	/**
	*	检测是否存在已经播放结束的buffer
	*	如果存在则decode新的数据
	*/
	ALint nb_empty;
	alGetSourcei(mSource, AL_BUFFERS_PROCESSED, &nb_empty);
	
	for (int i = 0; i < nb_empty; i++)
	{
		ALuint buffer;
		alSourceUnqueueBuffers(mSource, 1, &buffer);
		mOggDecoder->Decode(buffer, 16384);
		alSourceQueueBuffers(mSource, 1, &buffer);
	}	

	ALint status;
	alGetSourcei(mSource, AL_SOURCE_STATE, &status);
	if (status != AL_PLAYING)
	{
		alSourcePlay(mSource);
	}
}

void Sound::Load()
{
	std::string soundPath = "sounds/" + mSoundID;
	if (!FileData::IsFileExists(soundPath))
	{
		Debug::Error("The sound:" + soundPath + "is not exists.");
	}
	const std::string oggData = FileData::ReadFile(soundPath);

	/*** testing decoder **/

	alGenBuffers(numBuffers, mBuffers);
	alGenSources(1, &mSource);
	alSourcef(mSource, AL_GAIN, 1.0f);

	// decode
	mOggDecoder->Load(oggData);
	for (int i = 0; i < numBuffers; i++)
	{
		mOggDecoder->Decode(mBuffers[i], 16384);
	}

	// 播放
	alSourceQueueBuffers(mSource, numBuffers, mBuffers);
	alSourcePlay(mSource);
}

ALuint Sound::DecodeFile(const std::string file)
{
	return AL_NONE;
}

void Sound::Play()
{
}

//Sound * Sound::LoadFile(const std::string soundID)
//{
//	auto it = mSounds.find(soundID);
//	if (it != mSounds.end())
//	{
//		return &it->second;
//	}
//	mSounds[soundID] = Sound(soundID);
//	return &mSounds[soundID];
//}

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

	// init decoder
	mOggDecoder = std::unique_ptr<OggDecoder>(new OggDecoder);

	initialized = true;
}

void Sound::Update()
{

}

void Sound::Quid()
{
	if (initialized)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
		device = nullptr;
		context = nullptr;

		initialized = false;
	}
}
