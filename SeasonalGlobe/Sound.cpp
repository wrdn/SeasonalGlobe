#include "Sound.h"

Sound::Sound(void)
{
}


Sound::~Sound(void)
{
	Unload();
}

bool Sound::Load(const c8* filename)
{
	Unload();

	bufferID = alutCreateBufferFromFile(filename);
	if(bufferID == AL_NONE) return false;

	alGenSources(1, &sourceID);
	if(sourceID == AL_NONE) return false;

	SetParameteri(AL_BUFFER, bufferID);
	SetParameterf(AL_GAIN, 1.0f);

	return true;
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