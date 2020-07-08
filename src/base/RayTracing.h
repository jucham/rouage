#ifndef RAYTRACING_H
#define RAYTRACING_H

class Vector2D;
class Map;

class RayTracing
{
   public:

      // indicates if we can trace a ray between two position.
      // only solid tiles prevent ray to pass
      static bool canTraceRay(const Vector2D& rayOrigin,
                              const Vector2D& targPos,
                              const Map* pMap);
};

#endif // RAYTRACING_H
