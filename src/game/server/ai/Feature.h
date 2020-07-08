#ifndef FEATURE_H
#define FEATURE_H


class Bot;


///
/// \class Feature Feature.h
/// \brief class that implements methods to extract feature specific information
/// from the game world
///
/// class that implements methods to extract feature specific
/// information from the game world and present it as
/// a value in the range 0 to 1.
///
class Feature
{
   public:

      /// \brief returns a value between 0 and 1 based on the bot's health
      ///
      /// returns a value between 0 and 1 based on the bot's health. The better
      /// the health, the higher the rating
      static float Health(Bot* pBot);

      /// \brief returns a value between 0 and 1 based on the bot's closeness to the
      /// given item.
      static float DistanceToItem(Bot* pBot, int ItemType, int type);

      /// \brief returns a value between 0 and 1 based on how much ammo the bot has for
      /// the given weapon, and the maximum amount of ammo the bot can carry
      static float IndividualWeaponStrength(Bot* pBot, int WeaponType);

      /// \brief returns a value between 0 and 1 based on the total amount of ammo the
      /// bot is carrying each of the weapons.
      static float TotalWeaponStrength(Bot* pBot);
};

#endif // FEATURE_H
