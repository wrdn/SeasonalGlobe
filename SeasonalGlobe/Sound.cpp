#include "Sound.h"
#include "strutils.h"

#include <vorbisfile.h>

Sound::Sound(void) : bufferID(AL_NONE), sourceID(AL_NONE)
{
}


Sound::~Sound(void)
{
	Unload();
}

bool Sound::LoadWAV(const std::string &filename)
{
	Unload();

	bufferID = alutCreateBufferFromFile(filename.c_str());
	if(bufferID == AL_NONE)
	{
		return false;
	}

	alGenSources(1, &sourceID);
	if(sourceID == AL_NONE)
	{
		Unload();
		return false;
	}

	SetParameteri(AL_BUFFER, bufferID);
	SetParameterf(AL_GAIN, 1.0f);

	return true;
};

bool Sound::LoadOGG(const std::string &filename)
{
	Unload();

	alGenBuffers(1, &bufferID);
	if(bufferID == AL_NONE) return false;
    alGenSources(1, &sourceID);
	if(sourceID == AL_NONE) return false;

	const u32 BUFFER_SZ = 32768;

	i32 endian = 0;
	i32 bitStream;
	long bytes;
	c8 dataArray[BUFFER_SZ];

	FILE *fh = fopen(filename.c_str(), "rb");
	if(!fh) { return false; }

	vorbis_info *pInfo;
    OggVorbis_File oggFile;

    // Try opening the given file
	if (ov_open(fh, &oggFile, NULL, 0) != 0)
	{
		fclose(fh);
		return false;
	}

	ALenum format = AL_FORMAT_STEREO16;
	pInfo = ov_info(&oggFile, -1);
	if (pInfo->channels == 1)
        format = AL_FORMAT_MONO16;

	ALsizei freq = pInfo->rate;

	std::vector<c8> buff;

	// Read in the OGG data
	do
	{
		// Read up to a buffer's worth of decoded sound data
		bytes = ov_read(&oggFile, dataArray, BUFFER_SZ, endian, 2, 1, &bitStream);

		if (bytes < 0)
		{
			ov_clear(&oggFile);
			fclose(fh);
			return false;
		}
		// end if

		// Append to end of buffer
		buff.insert(buff.end(), dataArray, dataArray + bytes);
	}
	while (bytes > 0);
	
	fclose(fh);
	
	alBufferData(bufferID, format, &buff[0], static_cast<ALsizei>(buff.size()), freq);

	SetParameteri(AL_BUFFER, bufferID);
	SetParameterf(AL_GAIN, 1.0f);

	return true;
};

bool Sound::Load(const std::string &filename)
{
	// valid filename needs .wav or .ogg extension
	// 4 letter extension, plus a minimum of 1 letter filename
	if(!filename.size() || filename.size()<5) return false;

	size_t dotPos = filename.find_last_of('.');
	if(dotPos == std::string::npos)
	{
		return false; // no extension
	}

	std::string extension = filename.substr(dotPos+1);
	extension = strtoupper(extension);

	if(extension == "OGG")
	{
		return LoadOGG(filename);
	}
	else if(extension == "WAV")
	{
		return LoadWAV(filename);
	}
	
	return false; // unknown extension
};

void Sound::Unload()
{
	// delete buffer
	if(bufferID != AL_NONE)
	{
		alDeleteBuffers(1, &bufferID);
	}

	// delete source
	if(sourceID != AL_NONE)
	{
		alDeleteSources(1, &sourceID);
	}

	bufferID = sourceID = AL_NONE;
};

void Sound::SetParameteri(const ALenum param, const ALint val)
{
	alSourcei(sourceID, param, val);
};

void Sound::SetParameterf(const ALenum param, const ALfloat val)
{
	alSourcef(sourceID, param, val);
};

void Sound::SetParameter3f(const ALenum param, const ALfloat x,
	const ALfloat y, const ALfloat z)
{
	alSource3f(sourceID, param, x, y, z);
};

void Sound::SetParameter3f(const ALenum param, const ALfloat * const inVec)
{
	alSource3f(sourceID, param, inVec[0], inVec[1], inVec[2]);
};

ALint Sound::GetParameteri(const ALenum param) const
{
	ALint res=0;
	alGetSourcei(sourceID, param, &res);
	return res;
};

ALfloat Sound::GetParameterf(const ALenum param) const
{
	ALfloat res=0;
	alGetSourcef(sourceID, param, &res);
	return res;
};

void Sound::GetParameter3f(const ALenum param, ALfloat &out_x,
	ALfloat &out_y, ALfloat &out_z) const
{
	alGetSource3f(sourceID, param, &out_x, &out_y, &out_z);
};

void Sound::GetParameter3f(const ALenum param, ALfloat *outVec) const
{
	alGetSource3f(sourceID, param, &outVec[0], &outVec[1], &outVec[2]);
};

f32 Sound::GetVolume() const
{
	return GetParameterf(AL_GAIN);
};

void Sound::SetVolume(const f32 pVolume)
{
	SetParameterf(AL_GAIN, pVolume);
};

bool Sound::IsLooping() const
{
	return GetParameteri(AL_LOOPING) == AL_TRUE;
};

void Sound::SetLooping(const bool doLoop)
{
	SetParameteri(AL_LOOPING, (ALint)doLoop);
};

ALint Sound::GetSourceState() const
{
	return GetParameteri(AL_SOURCE_STATE);
};

void Sound::Play() const
{
	alSourcePlay(sourceID);
};

void Sound::Pause() const
{
	if(GetSourceState() == AL_PLAYING)
			alSourcePause(sourceID);
};

void Sound::Resume() const
{
	if(GetSourceState() == AL_PAUSED)
			Play();
};

void Sound::Stop() const
{
	if(GetSourceState() == AL_PLAYING)
			alSourceStop(sourceID);
};

bool Sound::Valid() const
{
	return bufferID != AL_NONE && sourceID != AL_NONE;
};