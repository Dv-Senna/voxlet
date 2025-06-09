cmake_minimum_required(VERSION 3.25)

find_package(Doxygen)
if (DOXYGEN_FOUND)
	set(VOXLET_DOC_DOXYGEN_IN  ${VOXLET_DOC_DIR}/Doxyfile.in)
	set(VOXLET_DOC_DOXYGEN_OUT ${VOXLET_DOC_GENERATED_DIR}/Doxyfile)

	configure_file(
		${VOXLET_DOC_DOXYGEN_IN}
		${VOXLET_DOC_DOXYGEN_OUT}
		@ONLY
	)

	message("Doxygen build started")

	add_custom_target(voxlet_doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${VOXLET_DOC_DOXYGEN_OUT}
		WORKING_DIRECTORY ${VOXLET_DOC_GENERATED_DIR}
		COMMENT "Generating Voxlet's documentation using Doxygen"
		VERBATIM
	)

else()
	message("You won't be able to generate documentation for Voxlet as you don't
		have Doxygen (or at least, CMake couldn't find it)"
	)
endif()
