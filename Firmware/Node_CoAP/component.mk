#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#



COMPONENT_EMBED_FILES := favicon.ico


target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "common/webserver/index.html" TEXT)
