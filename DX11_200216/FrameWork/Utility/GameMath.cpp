#include "Framework.h"

float GameMath::Random(const int& min, const int& max)
{
	return rand() % (max - min) + min;
}

float GameMath::Random(const float& min, const float& max)
{
	float normal = rand() / (float)RAND_MAX;

	return min + (max - min) * normal;
}
