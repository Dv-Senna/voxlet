#pragma once
/** @file */


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
	/**
	 * @brief A macro containing necessary attribute for proper function
	 *        calling depending in the platform
	 * */
	#define VOXLET_CORE
#endif
