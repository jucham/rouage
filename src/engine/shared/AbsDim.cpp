#include "AbsDim.h"

const float AbsDim::TILE_SIZE = 0.64f;
const float AbsDim::CHAR_W = 0.75f * AbsDim::TILE_SIZE;
const float AbsDim::CHAR_H = 2.75f * AbsDim::TILE_SIZE;
const float AbsDim::CHAR_ARM_W = 1.7f * AbsDim::CHAR_W;
const float AbsDim::CHAR_ARM_H = 0.7f * AbsDim::CHAR_W;
const float AbsDim::JETPACK_W = 1.6f * AbsDim::CHAR_W;
const float AbsDim::JETPACK_H = AbsDim::JETPACK_W;
const Dimension AbsDim::WPDIMS[NUM_WEAPONS] = {
      Dimension( 0.35f*AbsDim::CHAR_H, 0.75f*AbsDim::CHAR_W ),  // WP_PISTOL
      Dimension( 0.5f*AbsDim::CHAR_H, 1.2f*AbsDim::CHAR_W),     // WP_ROCKET_LAUNCHER
      Dimension( 2.0f*AbsDim::CHAR_W, 0.7f*AbsDim::CHAR_W ),    // WP_SHOTGUN
      Dimension( 3.0f*AbsDim::CHAR_W, 0.75f*AbsDim::CHAR_W )    // WP_AK47
};
const float AbsDim::ROCKET_W = 0.25f * AbsDim::WPDIMS[WP_ROCKET_LAUNCHER].height;
const float AbsDim::ROCKET_H = AbsDim::ROCKET_W;
const float AbsDim::BULLET_W = 0.2f * AbsDim::WPDIMS[WP_PISTOL].height;
const float AbsDim::BULLET_H = AbsDim::BULLET_W;
const float AbsDim::TRIGGER_W = AbsDim::TILE_SIZE;
const float AbsDim::TRIGGER_H = AbsDim::TILE_SIZE;
