#include "SFXAsset.h"
#include <stdio.h>
#include <cstdlib>
#include <cassert>
#include <base/DetectPlatform.h>
#include <base/debug.h>
#include "Sound.h"

extern "C" {
#include <engine/external/wavpack/wavpack.h>
}

FILE *SFXAsset::m_pFile = 0;

int read_data(void *buffer, int size)
{
	return fread(buffer, 1, size, SFXAsset::m_pFile);
}


SFXAsset::SFXAsset() :
		   m_SoundPtrs(NUM_SOUNDS, 0),
		   m_iMixingRate(44100)
{
	loadSound(SND_PISTOL_FIRE,          "data/sfx/pistol_fire.wv");
	loadSound(SND_SHOTGUN_FIRE,          "data/sfx/shotgun_fire.wv");
	loadSound(SND_AK47_FIRE_1,          "data/sfx/ak47_fire_1.wv");
	loadSound(SND_AK47_FIRE_2,          "data/sfx/ak47_fire_2.wv");
	loadSound(SND_AK47_FIRE_3,          "data/sfx/ak47_fire_3.wv");
	loadSound(SND_ROCKET_LAUNCHED,      "data/sfx/rocket_launched.wv");
		loadSound(SND_ROCKET_EXPLODE,       "data/sfx/rocket_explode.wv");
	loadSound(SND_WEAPON_VOID_1,        "data/sfx/weapon_void_1.wv");
	loadSound(SND_WEAPON_VOID_2,        "data/sfx/weapon_void_2.wv");
	loadSound(SND_WEAPON_PICKUP,        "data/sfx/weapon_pickup.wv");
	 	 loadSound(SND_WEAPON_CHANGE,        "data/sfx/weapon_change.wv");
	loadSound(SND_CLASSIC_CARTRIDGE_1,   "data/sfx/cartridge_impact_1.wv");
	loadSound(SND_CLASSIC_CARTRIDGE_2,   "data/sfx/cartridge_impact_2.wv");
	loadSound(SND_CLASSIC_CARTRIDGE_3,   "data/sfx/cartridge_impact_3.wv");

	loadSound(SND_SHOTGUN_CARTRIDGE_1,   "data/sfx/shotgun_cartridge_1.wv");
	loadSound(SND_SHOTGUN_CARTRIDGE_2,   "data/sfx/shotgun_cartridge_2.wv");
	loadSound(SND_SHOTGUN_CARTRIDGE_3,   "data/sfx/shotgun_cartridge_3.wv");
	loadSound(SND_SHOTGUN_CARTRIDGE_4,   "data/sfx/shotgun_cartridge_4.wv");
	loadSound(SND_BULLET_RICOCHET_1,    "data/sfx/bullet_ricochet_1.wv");
	loadSound(SND_BULLET_RICOCHET_2,    "data/sfx/bullet_ricochet_2.wv");
	loadSound(SND_BULLET_RICOCHET_3,    "data/sfx/bullet_ricochet_3.wv");

	loadSound(SND_ITEM_PICKUP,          "data/sfx/item_pickup.wv");

	loadSound(SND_STEP_HIGH_1,               "data/sfx/step_high_1.wv");
	loadSound(SND_STEP_HIGH_2,               "data/sfx/step_high_2.wv");
	loadSound(SND_STEP_HIGH_3,               "data/sfx/step_high_3.wv");
	loadSound(SND_STEP_HIGH_4,               "data/sfx/step_high_4.wv");
	loadSound(SND_STEP_LOW_1,               "data/sfx/step_low_1.wv");
	loadSound(SND_STEP_LOW_2,               "data/sfx/step_low_2.wv");
	loadSound(SND_STEP_LOW_3,               "data/sfx/step_low_3.wv");
	loadSound(SND_STEP_LOW_4,               "data/sfx/step_low_4.wv");
	loadSound(SND_JUMP,                       "data/sfx/jump.wv");
	loadSound(SND_JETPACK,                       "data/sfx/jetpack.wv");
	loadSound(SND_LANDING,              "data/sfx/landing.wv");
		loadSound(SND_DEATH_1,              "data/sfx/death_1.wv");
		loadSound(SND_DEATH_2,              "data/sfx/death_2.wv");
	//loadSound(SND_DEATH_3,              "data/sfx/death_3.wv");

	loadSound(SND_GIB_BIG_SPLATTER,              "data/sfx/gib_splatter.wv");
	loadSound(SND_GIB_IMPACT_1,              "data/sfx/gib_impact_1.wv");
	loadSound(SND_GIB_IMPACT_2,              "data/sfx/gib_impact_2.wv");
	loadSound(SND_GIB_IMPACT_3,              "data/sfx/gib_impact_3.wv");

	loadSound(SND_PAIN_1,              "data/sfx/pain_1.wv");
	loadSound(SND_PAIN_2,              "data/sfx/pain_2.wv");

//	loadSound(SND_MUSIC_1,              "data/sfx/music_1.wv");
}

SFXAsset::~SFXAsset()
{
	for(size_t i=0; i < m_SoundPtrs.size(); ++i)
	{
		delete m_SoundPtrs[i];
	}
}

const Sound* SFXAsset::GetSound(int soundId) const
{
	return m_SoundPtrs[soundId];
}

void SFXAsset::loadSound(int soundId, const char* filename)
{
	char error[100];
	WavpackContext *context;

	m_pFile = fopen(filename, "rb");

	if(m_pFile == 0)
	{
		printf("failed to open : %s\n", filename);
		exit(1);
	}

	// creates WAVPACK context
	context = WavpackOpenFileInput(read_data, error);

	if (context != 0)
	{
		int numSamples          = WavpackGetNumSamples(context);
		int bitsPerSample       = WavpackGetBitsPerSample(context);
		unsigned int sampleRate = WavpackGetSampleRate(context);
		int numChannels         = WavpackGetNumChannels(context);

		// mono or stereo
		if(numChannels > 2)
		{
			printf("file is not mono or stereo. filename=%s\n", filename);
			exit(1);
		}

		if(sampleRate != 44100)
		{
			printf("file is %d Hz, not 44100 Hz. filename=%s\n", sampleRate, filename);
			exit(1);
		}

		if(bitsPerSample != 16)
		{
			printf("bps is %d, not 16, filename=%s\n", bitsPerSample, filename);
			exit(1);
		}

		// allocate memory for raw data ( one sample = 32 bits = 4 bytes )
		int* pRawData = new int[4*numSamples*numChannels];

		// read decoded samples
		WavpackUnpackSamples(context, pRawData, numSamples);

		// allocates memory for real size of samples ( real sample size = 16 bits = 2 bytes )
		short* pData = new short[2*numSamples*numChannels];

		// copy raw data with casting to get short, the well size of samples
		int   *src = pRawData;
		short *dst = pData;
		for (int i=0; i < numSamples*numChannels; i++) { *dst++ = (short)*src++; }

		delete[] pRawData;

		m_SoundPtrs[soundId] = new Sound(pData, numSamples, sampleRate, numChannels);
	}
	else
	{
		printf("Can't create wavpack context for sound : %s\n", filename);
		exit(1);
	}

	fclose(m_pFile);
	m_pFile = 0;
}

void SFXAsset::convertSampleRate(int soundId)
{
	Sound* pSound = m_SoundPtrs[soundId];
	short* curData = pSound->Data();

	// make sure that we need to convert this sound
	if( curData == 0 || pSound->SampleRate() == m_iMixingRate)
		return;

	int numSamples = static_cast<int> ((pSound->NumSamples() / static_cast<float>(pSound->SampleRate()) * m_iMixingRate));
	short* newData = new short[numSamples*pSound->NumChannels()];

	for(int i=0; i < numSamples; i++)
	{
		float a = i / static_cast<float> (numSamples);
		int f = static_cast<int> (a*pSound->NumSamples());

		if (f >= pSound->NumSamples()) { f = pSound->NumSamples() - 1; }

		if(pSound->NumChannels() == 1)
		{
			newData[i] = curData[f];
		}
		else if(pSound->NumChannels() == 2)
		{
			newData[i*2] = curData[f*2];
			newData[i*2+1] = curData[f*2+1];
		}
	}

	delete[] curData;
	curData = newData;
	pSound->setNumSamples( numSamples );
}


