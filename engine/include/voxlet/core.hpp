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


#ifndef VOXLET_RENDERER_ENABLED_COUNT
	#error Something seems to be wrong with your configuration. `VOXLET_RENDERER_ENABLED_COUNT`\
		**must** be defined
	#include <stop_compilation>
#endif

#if VOXLET_RENDERER_ENABLED_COUNT == 1
	#define VOXLET_GRAPHICS_VIRTUAL 
	#define VOXLET_GRAPHICS_OVERRIDE
#else
	#define VOXLET_GRAPHICS_VIRTUAL virtual
	#define VOXLET_GRAPHICS_OVERRIDE override
#endif
