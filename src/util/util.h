#pragma once
#include "glm.hpp"

#include <string>
#ifdef _WIN32
#include <Windows.h>
#endif

inline glm::vec3 Lerp(glm::vec3 start, glm::vec3 end, float amount)
{
	return (start + amount * (end - start));
}

inline float Lerp(float start, float end, float amount)
{
	return (start + amount * (end - start));
}

inline std::string ExePath()
{
#ifdef _WIN32
	char buffer[MAX_PATH];
	GetModuleFileName( NULL, buffer, MAX_PATH );
	std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
	return std::string( buffer ).substr( 0, pos );
#else
	#error Unsupported platform for ExePath().
#endif
}