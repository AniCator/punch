#include "glm.hpp"

inline glm::vec3 Lerp(glm::vec3 start, glm::vec3 end, float amount)
{
	return (start + amount * (end - start));
}

inline float Lerp(float start, float end, float amount)
{
	return (start + amount * (end - start));
}