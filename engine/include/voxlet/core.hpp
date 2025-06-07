#pragma once


#ifdef VOXLET_SHARED_ENGINE
	#ifdef _WIN32
		#ifdef VOXLET_BUILD_ENGINE
			#define VOXLET_CORE __declspec(dllexport)
		#else
			#define VOXLET_CORE __declspec(dllimport)
		#endif
	#else
		#define VOXLET_CORE
	#endif
#else
	#define VOXLET_CORE
#endif
