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
		oggMem->position = (size_t)offset;
		break;
	case SEEK_CUR:
		oggMem->position += (size_t)offset;
		break;
	case SEEK_END:
		oggMem->position = oggMem->data.size() - (size_t)offset;
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

std::map<std::string, Sound> Sound::mAllSounds;
std::list<Sound*> Sound::mCurrentSounds;

Sound::Sound() :
	mSoundID(""),
	mBuffer(AL_NONE),
	mVolume(1.0f)
{
}

Sound::Sound(const std::string soundID) :
	mSoundID(soundID),
	mBuffer(AL_NONE),
	mVolume(1.0f)
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
		mCurrentSounds.remove(this);
	}
}

/**
*	\brief 更新当前的sound
*/
bool Sound::UpdatePlay()
{
	// 是否所有的source都播放完
	if (mSources.empty())
		return false;

	// 判断当前source是否播放完
	auto source = *mSources.begin();
	ALint state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	if (state != AL_PLAYING)
	{
		mSources.pop_front();
		alSourcei(source, AL_BUFFER, 0);
		alDeleteSources(1, &source);
	}

	return !mSources.empty();
}

/**
*	\brief 加载当前音效
*/
void Sound::Load()
{
	std::string soundPath = "sounds/" + mSoundID;
	if (!FileData::IsFileExists(soundPath))
	{
		Debug::Error("The sound:" + soundPath + "is not exists.");
	}

	/** 加载buffer */
	mBuffer = DecodeFile(soundPath);
}

ALuint Sound::DecodeFile(const std::string& filePath)
{
	ALuint buffer = AL_NONE;
	OggMemory mem;
	mem.loop = false;
	mem.position = 0;
	mem.data = FileData::ReadFile(filePath);

	// 基于OggVorbis的ogg格式解析
	OggVorbis_File file;
	int error = ov_open_callbacks(&mem, &file, nullptr, 0, mCallBacks);
	if (error)
	{
		std::ostringstream oss;
		oss << "Cannot load sound file '" << filePath;
		Debug::Error(oss.str());
		return AL_NONE;
	}

	// 解析ogg文件属性
	vorbis_info* info = ov_info(&file, -1);
	ALsizei sampleRate = ALsizei(info->rate);
	ALenum format = AL_NONE;
	if (info->channels == 1)
		format = AL_FORMAT_MONO16;
	else if (info->channels == 2)
		format = AL_FORMAT_STEREO16;

	if (format == AL_NONE)
	{
		Debug::Error("Invalid sound format.");
		return AL_NONE;
	}

	// 解析ogg文件流
	std::vector<char> samples;
	int bitStream;
	long bytesRead;
	long totalBytesRead = 0;
	char samplesBuffer[16384];
	do 	{
		bytesRead = ov_read(&file, samplesBuffer, 16384, 0, 2, 1, &bitStream);
		if (bytesRead < 0)
		{
			Debug::Error("Error while decoding ogg chunk.");
			return AL_NONE;
		}
		totalBytesRead += bytesRead;
		samples.insert(samples.end(), samplesBuffer, samplesBuffer + bytesRead);
	} 
	while (bytesRead > 0);

	// 创建buffer
	alGenBuffers(1, &buffer);
	if (alGetError() != AL_NO_ERROR)
		Debug::Error("Failed to generate buffer");

	alBufferData(buffer, AL_FORMAT_STEREO16,
		reinterpret_cast<ALshort*>(samples.data()),
		ALsizei(totalBytesRead), sampleRate);

	if (alGetError() != AL_NO_ERROR)
		Debug::Error("Failed to bind buffer");

	// clear
	ov_clear(&file);

	return buffer;
}

/**
*	\brief 播放当前音效
*/
void Sound::Play()
{
	if (initialized)
	{
		if (mBuffer == AL_NONE)
			Load();

		if (mBuffer != AL_NONE)
		{
			// 创建source，并设置buffer
			ALuint source;
			alGenSources(1, &source);
			alSourcei(source, AL_BUFFER, mBuffer);
			alSourcef(source, AL_GAIN, mVolume);
			
			if (alGetError() != AL_NO_ERROR)
			{
				Debug::Error("Failed to attach sound buffer '" + mSoundID + "'");
				alDeleteSources(1, &source);
				return;
			}

			mSources.push_back(source);
			mCurrentSounds.remove(this);
			mCurrentSounds.push_back(this);
			
			alSourcePlay(source);

			if (alGetError() != AL_NO_ERROR)
			{
				Debug::Error("Failed to play sound '" + mSoundID + "'");
				alDeleteSources(1, &source);
				return;
			}
		}
	}
}

Sound * Sound::LoadFile(const std::string soundID)
{
	auto it = mAllSounds.find(soundID);
	if (it != mAllSounds.end())
	{
		return &it->second;
	}
	mAllSounds[soundID] = Sound(soundID);
	return &mAllSounds[soundID];
}

/**
*	\brief 初始化OpenAL模块
*/
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

	ALCint attr[] = { ALC_FREQUENCY, 32000, 0 };
	context = alcCreateContext(device, attr);
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
	std::vector<Sound*> soundsToRemove;
	for (auto& sound : mCurrentSounds)
	{
		if (sound->UpdatePlay())
			soundsToRemove.push_back(sound);
	}

	for (auto& sound : soundsToRemove)
		mCurrentSounds.remove(sound);
	soundsToRemove.clear();

	Music::Update();
}

void Sound::Quit()
{
	if (initialized)
	{
		Music::Quid();

		mAllSounds.clear();

		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context);
		alcCloseDevice(device);
		device = nullptr;
		context = nullptr;

		initialized = false;
	}
}

/**
*	\brief 播放指定的sound
*/
void Sound::PlaySound(const std::string & id)
{
	if (mAllSounds.find(id) == mAllSounds.end())
		mAllSounds[id] = Sound(id);

	mAllSounds[id].Play();
}
