#pragma once

#include "GameResource.h"
#include <string>
#include <alut.h> // ALUT (OpenAL Utility Toolkit)
#include <al.h>   // OpenAL

class Sound : public Resource
{
private:
	u32 bufferID, sourceID;

	bool LoadWAV(const std::string &filename);
	bool LoadOGG(const std::string &filename);

public:
	Sound(void);
	~Sound(void);

	bool Load(const std::string &filename);

	void Unload();

	// set integer paramter
	void SetParameteri(const ALenum param, const ALint val);

	// set float paramter
	void SetParameterf(const ALenum param, const ALfloat val);

	// set 3D float paramter (through individual variables)
	void SetParameter3f(const ALenum param, const ALfloat x,
		const ALfloat y, const ALfloat z);

	// set 3D float paramter (through array) - useful if using float3/float4, as you can
	// pass vec.GetVec() as the inVec argument
	void SetParameter3f(const ALenum param, const ALfloat * const inVec);

	// get integer parameter
	ALint GetParameteri(const ALenum param) const;

	// get float paramter
	ALfloat GetParameterf(const ALenum param) const;

	// get 3D float paramter into individual variables
	void GetParameter3f(const ALenum param, ALfloat &out_x,
		ALfloat &out_y, ALfloat &out_z) const;

	// get 3D float parameter into array
	void GetParameter3f(const ALenum param, ALfloat *outVec) const;

	// volume control
	f32 GetVolume() const;
	void SetVolume(const f32 pVolume);

	// looping control
	bool IsLooping() const;
	void SetLooping(const bool doLoop);

	// state of source e.g. playing, paused, stopped
	ALint GetSourceState() const;

	// playback control
	void Play() const;
	void Pause() const;
	void Resume() const;
	void Stop() const;

	bool Valid() const; // returns true if bufferID and sourceID != AL_NONE
};

typedef std::tr1::shared_ptr<Sound> SoundHandle;