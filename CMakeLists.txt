############################################
#                                          #
#  Improvements and feedbacks are welcome  #
#                                          #
#  This file is released under GPL >= 3    #
#                                          #
############################################


cmake_minimum_required( VERSION 3.5 )


#### general package setup

project( tdeaccessibility )


#### include essential cmake modules

include( FindPkgConfig          )
include( CheckFunctionExists    )
include( CheckSymbolExists      )
include( CheckIncludeFile       )
include( CheckIncludeFiles      )
include( CheckLibraryExists     )
include( CheckCSourceCompiles   )
include( CheckCXXSourceCompiles )


#### include our cmake modules

include( TDEMacros )
enable_testing( )


##### set version number ########################

tde_set_project_version( )


##### setup install paths

include( TDESetupPaths )
tde_setup_paths( )


##### optional stuff

option( WITH_ALL_OPTIONS    "Enable all optional support" OFF )
option( WITH_GCC_VISIBILITY "Enable fvisibility and fvisibility-inlines-hidden" ${WITH_ALL_OPTIONS} )
option( WITH_ARTS           "Enable aRts support"                               ON                  )
option( WITH_AKODE          "Enable akode support"                              ${WITH_ALL_OPTIONS} )
option( WITH_ALSA           "Enable alsa support"                               ${WITH_ALL_OPTIONS} )
option( WITH_GSTREAMER      "Enable Gstreamer support"                          OFF                 )

########################################################################
#                                                                      #
#                   aRts support is needed to build:                   #
#                   - kttsd artsplayer                                 #
#                   - ksayit                                           #
#                   - ksayit freeverb plugin                           #
#                                                                      #
########################################################################


##### user requested modules

option( BUILD_ALL "Build all" ON )

option( BUILD_KBSTATEAPPLET   "Build kbstateapplet"          ${BUILD_ALL} )
option( BUILD_KMAG            "Build kmag"                   ${BUILD_ALL} )
option( BUILD_KMOUSETOOL      "Build kmousetool"             ${BUILD_ALL} )
option( BUILD_KMOUTH          "Build kmouth"                 ${BUILD_ALL} )
option( BUILD_KSAYIT          "Build ksayit"                 ${BUILD_ALL} )
option( BUILD_KSAYIT_FREEVERB "Build ksayit freeverb plugin" ${BUILD_ALL} )
option( BUILD_KTTSD           "Build kttsd"                  ${BUILD_ALL} )
option( BUILD_KTTSD_COMMAND   "Build kttsd command plugin"   ${BUILD_ALL} )
option( BUILD_KTTSD_EPOS      "Build kttsd epos plugin"      ${BUILD_ALL} )
option( BUILD_KTTSD_FESTIVAL  "Build kttsd festival plugin"  ${BUILD_ALL} )
option( BUILD_KTTSD_FLITE     "Build kttsd flite plugin"     ${BUILD_ALL} )
option( BUILD_KTTSD_FREETTS   "Build kttsd freetts plugin"   ${BUILD_ALL} )
option( BUILD_KTTSD_HADIFIX   "Build kttsd hadifix plugin"   ${BUILD_ALL} )
option( BUILD_KTTSD_KATE      "Build kttsd kate plugin"      ${BUILD_ALL} )
option( BUILD_ICONTHEMES      "Build icons"                  ${BUILD_ALL} )
option( BUILD_DOC             "Build documentation"          ${BUILD_ALL} )


##### configure checks

include( ConfigureChecks.cmake )


###### global compiler settings

add_definitions( -DHAVE_CONFIG_H )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TQT_CXX_FLAGS}" )
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
set( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined" )


##### directories

tde_conditional_add_subdirectory( BUILD_KBSTATEAPPLET kbstateapplet )
tde_conditional_add_subdirectory( BUILD_KMAG          kmag          )
tde_conditional_add_subdirectory( BUILD_KMOUSETOOL    kmousetool    )
tde_conditional_add_subdirectory( BUILD_KMOUTH        kmouth        )
tde_conditional_add_subdirectory( BUILD_KSAYIT        ksayit        )
tde_conditional_add_subdirectory( BUILD_KTTSD         kttsd         )
tde_conditional_add_subdirectory( BUILD_ICONTHEMES    IconThemes    )
tde_conditional_add_subdirectory( BUILD_DOC           doc           )

if( BUILD_KSAYIT OR BUILD_KTTSD )
    add_subdirectory( kspeechsink )
endif()


##### write configure files

configure_file( config.h.cmake config.h @ONLY )
