SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libs/${CMAKE_SYSTEM_NAME}")

SET(SOURCES all.h
            pcx.h pcx.cpp
            obj.h obj.cpp)

ADD_LIBRARY(converters STATIC ${SOURCES})
