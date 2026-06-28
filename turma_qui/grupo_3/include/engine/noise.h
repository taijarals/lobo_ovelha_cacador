#ifndef ENGINE_NOSE_H
#define ENGINE_NOSE_H

float whiteNoise2d(int seed, int x, int y);
float noise2d(int seed, float x, float y);
float perlin2d(int seed, float x, float y, float freq, int depth);

#endif //ENGINE_NOSE_H
