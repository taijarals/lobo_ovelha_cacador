#include <stdint.h>
#include <engine/noise.h>

#define INV_U32 (1.0f / 4294967295.0f)

static uint32_t hash(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7feb352d;
    x ^= x >> 15;
    x *= 0x846ca68b;
    x ^= x >> 16;
    return x;
}
static float lin_inter(float x, float y, float s) { return x + s * (y-x); }
static float smooth_inter(float x, float y, float s) { return lin_inter(x, y, s * s * (3-2*s)); }

float whiteNoise2d(int seed, int x, int y)
{
    uint32_t h = hash(hash(y + seed) + x);
    return h * INV_U32;
}

float noise2d(int seed, float x, float y)
{
    int x_int = x;
    int y_int = y;

    float x_frac = x - x_int;
    float y_frac = y - y_int;

    int s = whiteNoise2d(seed, x_int, y_int);
    int t = whiteNoise2d(seed, x_int+1, y_int);
    int u = whiteNoise2d(seed, x_int, y_int+1);
    int v = whiteNoise2d(seed, x_int+1, y_int+1);

    float low = smooth_inter(s, t, x_frac);
    float high = smooth_inter(u, v, x_frac);

    return smooth_inter(low, high, y_frac);
}

float perlin2d(int seed, float x, float y, float freq, int depth)
{
    float xa = x*freq;
    float ya = y*freq;
    float amp = 1.0;
    float fin = 0;
    float div = 0.0;

    int i;
    for(i=0; i<depth; i++)
    {
        div += 256 * amp;
        fin += noise2d(seed, xa, ya) * amp;
        amp /= 2;
        xa *= 2;
        ya *= 2;
    }

    return fin/div;
}
