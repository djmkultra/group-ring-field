###############################################################
##    _____________  ______________________    __   __  _____
##   /  ________  |  |   ___   ________   /   |  \ /  \   |
##  |  |       |  |_ |  |_ |  |       /  /    \__  |      |
##  |  |  ___  |  || |  || |  |      /  /        | |      |
##  |  | |   \ |  || |  || |  |     /  /      \__/ \__/ __|__
##  |  | |_@  ||  || |  || |  |    /  /          Institute
##  |  |___/  ||  ||_|  || |  |   /  /_____________________
##   \_______/  \______/ | |__|  /___________________________
##              |  |__|  |
##               \______/
##          University of Utah       
##                 2002
###############################################################

###############################################################
## Project Name: GutzLib
###############################################################

cmake_minimum_required(VERSION 2.4)

PROJECT(GutzLib)

# this next line tells GutzSources.cmake not to look for a lib
#  since this is the project that will be building it

SET(GUTZ_LIB_BUILD 1)

# Since we already are in the gutz directory, we don't need
#  to go looking for it

SET(GUTZ_PATH ${GutzLib_SOURCE_DIR})

# This is the file that contains all of the source information
#  about gutz...

INCLUDE(${GutzLib_SOURCE_DIR}/GutzSources.cmake)


###############################################################
## Include directories
##   Gutz is a top-level library, it depends on no one
###############################################################
INCLUDE_DIRECTORIES( ${GUTZ_INCLUDE_PATH}
                     ${QT_INCLUDES} )

###############################################################
## Create Library
###############################################################

ADD_LIBRARY( ${GUTZ_LIB} ${GUTZ_SOURCE_FILES} ${GUTZ_INCLUDE_FILES} )

###############################################################
## Install Library
##   Install GUTZ include files and library
###############################################################

INSTALL_FILES("/include" FILES ${GUTZ_INCLUDE_FILES})
INSTALL_TARGETS("/lib" ${GUTZ_LIB})

###############################################################
## TEST gutz
###############################################################

SET(GUTZ_TEST 1 CACHE BOOL "Build the gutz test programs?")

IF(GUTZ_TEST)
  SUBDIRS(test)
ENDIF(GUTZ_TEST)

###############################################################
## MSVC specific stuff, folders
###############################################################

IF(WIN32)
  #############################
  # MathGutz
  SOURCE_GROUP(MathGutz FILES ${MATH_GUTZ_SRC})
  #
  #############################

  #############################
  # ArrayGutz
  SOURCE_GROUP(ArrayGutz FILES ${ARRAY_GUTZ_SRC}) 
  #
  #############################

  #############################
  # ArrayGutz
  SOURCE_GROUP(GraphicsGutz FILES ${GRAPHICS_GUTZ_SRC})
  #
  #############################

  #############################
  # Signals & Slots
  SOURCE_GROUP(SignalGutz FILES ${SIGSLOT_SRC})
  #
  #############################

  #############################
  # Events
  SOURCE_GROUP(EventGutz FILES ${EVENT_SRC})
  #
  #############################

  #############################
  # Events
  SOURCE_GROUP(ThreadGutz FILES ${THREAD_SRC})
  #
  #############################

  #############################
  # Events
  SOURCE_GROUP(ExceptionGutz FILES ${EXCEPTION_SRC})
  #
  #############################

ENDIF(WIN32)
