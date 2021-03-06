ADD_SUBDIRECTORY(logs)
ADD_SUBDIRECTORY(languages)
ADD_SUBDIRECTORY(threads)
ADD_SUBDIRECTORY(misc)
ADD_SUBDIRECTORY(jpeg)
ADD_SUBDIRECTORY(gfx)
ADD_SUBDIRECTORY(network)
ADD_SUBDIRECTORY(ai)
ADD_SUBDIRECTORY(ingame)
ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(sounds)
ADD_SUBDIRECTORY(converters)
ADD_SUBDIRECTORY(animator)
IF(${TA3D_REGRESSION_TEST} EQUAL 1)
   ADD_SUBDIRECTORY(regression)
ENDIF(${TA3D_REGRESSION_TEST} EQUAL 1)


IF(APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
    SET(MACOSX_BUNDLE_BUNDLE_NAME "TA3D")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "ta3d")
ENDIF(APPLE)
LINK_LIBRARIES("${LIBS}")



# --------------------------------------------------------------------
# --- STDAFX
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(SOURCES stdafx.h stdafx.cpp)
ADD_LIBRARY(stdafx STATIC ${SOURCES})

# --------------------------------------------------------------------
# --- Debug Informations
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(SOURCES backtrace.cpp backtrace.h
            cError.cpp cError.h)
ADD_LIBRARY(debuginfos STATIC ${SOURCES})

# --------------------------------------------------------------------
# --- Console
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(SOURCES console.cpp console.h)
ADD_LIBRARY(console STATIC ${SOURCES})

# --------------------------------------------------------------------
# --- GUI
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(SOURCES gui.cpp gui.h)
ADD_LIBRARY(oldgui STATIC ${SOURCES})

# --------------------------------------------------------------------
# --- File formats
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(SOURCES gaf.cpp gaf.h
            3do.cpp 3do.h
            3ds.cpp 3ds.h)
ADD_LIBRARY(fileformats STATIC ${SOURCES})

# --------------------------------------------------------------------
# --- HPI
# --------------------------------------------------------------------
SET(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/libs/${CMAKE_SYSTEM_NAME}")
SET(SOURCES TA3D_hpi.cpp TA3D_hpi.h
            hpiview.cpp)
ADD_LIBRARY(hpi STATIC ${SOURCES})






# --------------------------------------------------------------------
# --- EDITOR
# --------------------------------------------------------------------


if( WIN32 OR CMAKE_CROSSCOMPILING )
	SET( 3DMEDITOR_BIN 3dmeditor )
else( WIN32 OR CMAKE_CROSSCOMPILING )
	SET( 3DMEDITOR_BIN 3dmeditor )
endif( WIN32 OR CMAKE_CROSSCOMPILING )

ADD_EXECUTABLE(${3DMEDITOR_BIN}
        3dmeditor.cpp ta3d.h 3dmeditor_sub.cpp 3dmeditor.h
        cTAFileParser.cpp cTAFileParser.h stdafx.h
        TA3D_NameSpace.cpp TA3D_NameSpace.h ta3dbase.h)
target_link_libraries(${3DMEDITOR_BIN}
        stdafx logs console debuginfos 3dtoolbox misc i18n
        tajpeg fileformats particles hpi oldgui gui ingame tdf
        converters animator gfx ta3dmath audio threads)
IF(APPLE)
    SET(GUI_TYPE MACOSX_BUNDLE)
    SET(MACOSX_BUNDLE_BUNDLE_NAME "TA3D Editor")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "ta3d-editor")
ENDIF(APPLE)



# --------------------------------------------------------------------
# --- TA3D
# --------------------------------------------------------------------
IF(APPLE)
	SET(TA3D_BIN ta3d)
ELSE(APPLE)
	SET(TA3D_BIN ta3d)
ENDIF(APPLE)
SET(SOURCES
        ta3d.h main.cpp install.cpp cTA3D_Engine.cpp cTA3D_Engine.h
        cTAFileParser.cpp cTAFileParser.h restore.cpp restore.h menu.cpp menu.h
        EngineClass.cpp EngineClass.h UnitEngine.cpp UnitEngine.h
        fbi.cpp fbi.h intro.cpp intro.h  tdf.cpp tdf.h tnt.cpp tnt.h 
        icon.cpp stdafx.h TA3D_NameSpace.cpp TA3D_NameSpace.h ta3dbase.h)



IF(NOT APPLE)
    ADD_EXECUTABLE(${TA3D_BIN} ${SOURCES})
ELSE(NOT APPLE)

	#
	# --- TA3D Program / OS XBundle ---
	#
	
	# OS X : Add a single file as a bundle resource
	MACRO(BUNDLE_INSTALL srcfolder relfolder filename)
		File(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/")
		ADD_CUSTOM_COMMAND(OUTPUT "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/${filename}"
			COMMAND cp ARGS -f "${PROJECT_SOURCE_DIR}/${srcfolder}/${filename}"
			  "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/${filename}"
			DEPENDS "${PROJECT_SOURCE_DIR}/${srcfolder}/${filename}") 
		SET(SOURCES ${SOURCES} "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/${filename}")
	ENDMACRO(BUNDLE_INSTALL)

	MACRO(BUNDLE_INSTALL_GLOB srcfolder relfolder globfiles)
		File(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/")
		File(GLOB lst "${PROJECT_SOURCE_DIR}/${srcfolder}/${globfiles}")
		ForEach(fl ${lst})
			String(REGEX REPLACE "^/.*/([^/]+)$" "\\1" fl "${fl}")
			ADD_CUSTOM_COMMAND(OUTPUT "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/${fl}"
				COMMAND cp ARGS -f "${PROJECT_SOURCE_DIR}/${srcfolder}/${fl}"
					"${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/${fl}"
				DEPENDS "${PROJECT_SOURCE_DIR}/${srcfolder}/${fl}") 
			SET(SOURCES ${SOURCES} "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/${relfolder}/${fl}")
		EndForEach(fl ${lst})
	ENDMACRO(BUNDLE_INSTALL_GLOB)

	MACRO(BUNDLE_BASIC_INSTALL srcfolder relfolder filename)
		File(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/${relfolder}/")
		ADD_CUSTOM_COMMAND(OUTPUT "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/${relfolder}/${filename}"
			COMMAND cp ARGS -f "${PROJECT_SOURCE_DIR}/${srcfolder}/${filename}" "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/${relfolder}/${filename}"
			DEPENDS "${PROJECT_SOURCE_DIR}/${srcfolder}/${filename}") 
		SET(SOURCES ${SOURCES} "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/${relfolder}/${filename}")
	ENDMACRO(BUNDLE_BASIC_INSTALL)

	File(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Resources/")
	File(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/")
	File(MAKE_DIRECTORY "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/MacOS/")

	# Ta3d mod
	BUNDLE_BASIC_INSTALL("./" "MacOS" "ta3d.mod")
	# Misc
	BUNDLE_BASIC_INSTALL("./" "./" "README")
	BUNDLE_BASIC_INSTALL("./" "./" "AUTHORS")
	# License
	BUNDLE_BASIC_INSTALL("./" "./" "COPYING")

    # FMOD
    ADD_CUSTOM_COMMAND(OUTPUT "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/libfmodex.dylib"
		COMMAND cp ARGS -f "${FMOD_LIB_DARWIN_FOLDER}/libfmodex.dylib" "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/libfmodex.dylib"
        DEPENDS "${FMOD_LIB_DARWIN_FOLDER}/libfmodex.dylib") 
    SET(SOURCES ${SOURCES} "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/libfmodex.dylib")

    # LibZ
    ADD_CUSTOM_COMMAND(OUTPUT "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/libz.1.dylib"
		COMMAND cp ARGS -fH "/opt/local/lib/libz.1.dylib" "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/libz.1.dylib"
        DEPENDS "/opt/local/lib/libz.1.dylib") 
    SET(SOURCES ${SOURCES} "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/libz.1.dylib")

    # Allegro
    ADD_CUSTOM_COMMAND(OUTPUT "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/liballeg-4.2.2.dylib"
		COMMAND cp ARGS -fH "/opt/local/lib/liballeg-4.2.2.dylib" "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/liballeg-4.2.2.dylib"
        DEPENDS "/opt/local/lib/liballeg-4.2.2.dylib") 
    SET(SOURCES ${SOURCES} "${PROJECT_SOURCE_DIR}/ta3d.app/Contents/Frameworks/liballeg-4.2.2.dylib")

	# --- Resources --- 
	# Sky
	Message(STATUS "[bundle] Adding resources 'sky'...")
	BUNDLE_INSTALL_GLOB("sky" "sky" "*.tdf")
	# Shaders
	Message(STATUS "[bundle] Adding resources 'shaders'...")
	BUNDLE_INSTALL_GLOB("shaders" "shaders" "*.vert")
	BUNDLE_INSTALL_GLOB("shaders" "shaders" "*.frag")
	# ai
	Message(STATUS "[bundle] Adding resources 'ai'...")
	BUNDLE_INSTALL_GLOB("ai" "ai" "*.ai")
	# Gfx
	Message(STATUS "[bundle] Adding resources 'gfx'...")
	BUNDLE_INSTALL_GLOB("gfx" "gfx" "*.jpg")
	BUNDLE_INSTALL_GLOB("gfx" "gfx" "*.png")
	BUNDLE_INSTALL_GLOB("gfx" "gfx" "*.tga")
	BUNDLE_INSTALL_GLOB("gfx/default_skin" "gfx/default_skin" "*.tga")
	BUNDLE_INSTALL_GLOB("gfx/default_skin" "gfx/default_skin" "*.jpg")
	BUNDLE_INSTALL_GLOB("gfx/default_skin" "gfx/default_skin" "*.png")
	BUNDLE_INSTALL_GLOB("gfx/mdrn_skin" "gfx/mdrn_skin" "*.tga")
	BUNDLE_INSTALL_GLOB("gfx/mdrn_skin" "gfx/mdrn_skin" "*.jpg")
	BUNDLE_INSTALL_GLOB("gfx/mdrn_skin" "gfx/mdrn_skin" "*.png")
	BUNDLE_INSTALL_GLOB("gfx/mdrn_teams" "gfx/mdrn_teams" "*.tga")
	BUNDLE_INSTALL_GLOB("gfx/mdrn_teams" "gfx/mdrn_teams" "*.jpg")
	BUNDLE_INSTALL_GLOB("gfx/mdrn_teams" "gfx/mdrn_teams" "*.png")
	BUNDLE_INSTALL_GLOB("gfx/sky" "gfx/sky" "*.jpg")
	BUNDLE_INSTALL_GLOB("gfx/teams" "gfx/teams" "*.tga")
	BUNDLE_INSTALL_GLOB("gfx/tactical icons" "gfx/tactical icons/" "*.tga")
	# gui
	Message(STATUS "[bundle] Adding resources 'gui'...")
	BUNDLE_INSTALL_GLOB("gui" "gui" "*.tdf")
	BUNDLE_INSTALL_GLOB("gui" "gui" "*.area")
	BUNDLE_INSTALL_GLOB("gui" "gui" "*.skn")
	# Mods
	Message(STATUS "[bundle] Adding resources 'mods'...")
	BUNDLE_INSTALL("mods/ta3d" "mods/ta3d" "files.txt")
	BUNDLE_INSTALL_GLOB("mods/ta3d/objects3d" "mods/ta3d/objects3d" "*.3dm")
	BUNDLE_INSTALL_GLOB("mods/ta3d/objects3d" "mods/ta3d/objects3d" "*.3do")
	# objects3d
	Message(STATUS "[bundle] Adding resources 'objects3d'...")
	BUNDLE_INSTALL_GLOB("objects3d" "objects3d" "*.3dm")
	BUNDLE_INSTALL_GLOB("objects3d" "objects3d" "*.3do")
	# Scripts
	Message(STATUS "[bundle] Adding resources 'scripts'...")
	BUNDLE_INSTALL_GLOB("scripts" "scripts" "*.lua")
	BUNDLE_INSTALL("scripts" "scripts" "signals.h")
	# Pictures
	Message(STATUS "[bundle] Adding resources 'pictures'...")
	BUNDLE_INSTALL_GLOB("pictures" "pictures" "*.jpg")
	BUNDLE_INSTALL_GLOB("pictures" "pictures" "*.ico")
	BUNDLE_INSTALL_GLOB("pictures" "pictures" "*.png")
	BUNDLE_INSTALL_GLOB("pictures" "pictures" "*.xcf")
	BUNDLE_INSTALL_GLOB("pictures" "pictures" "*.tga")
	BUNDLE_INSTALL_GLOB("pictures" "pictures" "*.icns")
	# Intro 
	Message(STATUS "[bundle] Adding resources 'intro'...")
	BUNDLE_INSTALL_GLOB("resources/intro" "intro" "*.txt")
	# Translations
	Message(STATUS "[bundle] Adding resources 'languages'...")
	BUNDLE_INSTALL_GLOB("resources/languages" "languages" "*.txt")

	# *.res
	BUNDLE_BASIC_INSTALL("resources" "./" "ta3d.res")
	BUNDLE_BASIC_INSTALL("resources" "./" "3dmeditor.res")

    # OS X specific
    ADD_EXECUTABLE(${TA3D_BIN} MACOSX_BUNDLE ${SOURCES})
    SET(GUI_TYPE MACOSX_BUNDLE)
    SET(MACOSX_BUNDLE_BUNDLE_NAME "TA3D")
    SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "ta3d")
    SET(MACOSX_BUNDLE_ICON_FILE "pictures/ta3d.icns")

    # Relink (needed to make an independant Bundle)
    ADD_CUSTOM_COMMAND(TARGET ${TA3D_BIN} POST_BUILD
        COMMAND install_name_tool ARGS -change './libfmodex.dylib' @executable_path/../Frameworks/libfmodex.dylib ${PROJECT_SOURCE_DIR}/ta3d.app/Contents/MacOS/ta3d )
    ADD_CUSTOM_COMMAND(TARGET ${TA3D_BIN} POST_BUILD
        COMMAND install_name_tool ARGS -change '/opt/local/lib/libz.1.dylib' @executable_path/../Frameworks/libz.1.dylib ${PROJECT_SOURCE_DIR}/ta3d.app/Contents/MacOS/ta3d )
    ADD_CUSTOM_COMMAND(TARGET ${TA3D_BIN} POST_BUILD
        COMMAND install_name_tool ARGS -change '/opt/local/lib/liballeg-4.2.2.dylib' @executable_path/../Frameworks/liballeg-4.2.2.dylib ${PROJECT_SOURCE_DIR}/ta3d.app/Contents/MacOS/ta3d )

ENDIF(NOT APPLE)


#
# --- Internal libraries ---
#
target_link_libraries(${TA3D_BIN}
        stdafx logs console i18n ingame 3dtoolbox menus tajpeg gfx
        fileformats particles weapons ai audio hpi oldgui gui network misc scripts ta3dmath tdf
        converters threads debuginfos)




#
# --- External libraries
#   
IF( VAR_LIB_FMODEX )
   target_link_libraries(${TA3D_BIN} fmodex)
   target_link_libraries(${3DMEDITOR_BIN} fmodex)
elseif( VAR_LIB_FMODEX64 )
   target_link_libraries(${TA3D_BIN} fmodex64)
   target_link_libraries(${3DMEDITOR_BIN} fmodex64)
else( VAR_LIB_FMODEX )
   message( FATAL_ERROR "FMODex is required, please install FMODex" )
endif( VAR_LIB_FMODEX )

IF(WIN32)
    target_link_libraries(${3DMEDITOR_BIN} agl lua gdi32 alleg opengl32 glu32)
    target_link_libraries(${TA3D_BIN} agl lua gdi32 alleg opengl32 glu32 NL)
ENDIF(WIN32)

IF(NOT APPLE)
	install(TARGETS ${TA3D_BIN} RUNTIME DESTINATION ${TA3D_BINARIES_INSTALL_PATH})
	install(TARGETS ${3DMEDITOR_BIN} RUNTIME DESTINATION ${TA3D_BINARIES_INSTALL_PATH})
ENDIF(NOT APPLE)

