SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/libs/${CMAKE_SYSTEM_NAME}")


SET(SOURCES logs.h logs.hxx logs.cpp)

ADD_LIBRARY(logs STATIC ${SOURCES})
