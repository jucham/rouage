#ifndef SFXASSET_H
#define SFXASSET_H

#include <stdio.h>
#include <vector>

class Sound;

enum
{
	SND_SILENCE,
	SND_PISTOL_FIRE,
	SND_SHOTGUN_FIRE,
	SND_AK47_FIRE_1,
	SND_AK47_FIRE_2,
	SND_AK47_FIRE_3,
	SND_ROCKET_LAUNCHED,
	SND_ROCKET_EXPLODE,
	SND_WEAPON_VOID_1,
	SND_WEAPON_VOID_2,
	SND_WEAPON_PICKUP,
	SND_WEAPON_CHANGE,
	SND_CLASSIC_CARTRIDGE_1,
	SND_CLASSIC_CARTRIDGE_2,
	SND_CLASSIC_CARTRIDGE_3,
	SND_SHOTGUN_CARTRIDGE_1,
	SND_SHOTGUN_CARTRIDGE_2,
	SND_SHOTGUN_CARTRIDGE_3,
	SND_SHOTGUN_CARTRIDGE_4,
	SND_BULLET_RICOCHET_1,
	SND_BULLET_RICOCHET_2,
	SND_BULLET_RICOCHET_3,

	SND_ITEM_PICKUP,

	SND_STEP_HIGH_1,
	SND_STEP_HIGH_2,
	SND_STEP_HIGH_3,
	SND_STEP_HIGH_4,
	SND_STEP_LOW_1,
	SND_STEP_LOW_2,
	SND_STEP_LOW_3,
	SND_STEP_LOW_4,
	SND_JUMP,
	SND_JETPACK,
	SND_LANDING,
	SND_DEATH_1,
	SND_DEATH_2,
	//SND_DEATH_3,

	SND_GIB_BIG_SPLATTER,
	SND_GIB_IMPACT_1,
	SND_GIB_IMPACT_2,
	SND_GIB_IMPACT_3,

	SND_PAIN_1,
	SND_PAIN_2,

	SND_MUSIC_1

};

enum
{
	NUM_SOUNDS=64
};


class SFXAsset
{
public:
	const Sound* GetSound(int soundId) const;

	static SFXAsset & Instance() {
		static SFXAsset instance;
		return instance;
	}

	static FILE *m_pFile;

private:

	SFXAsset();
	~SFXAsset();
	SFXAsset(const SFXAsset &);
	SFXAsset & operator=(const SFXAsset &);

	void loadSound(int soundId, const char* filename);
	void convertSampleRate(int soundId);

	friend int read_data(void *buffer, unsigned size);

	std::vector<Sound*> m_SoundPtrs;
	int m_iMixingRate;
};

#endif // SFXASSET_H
