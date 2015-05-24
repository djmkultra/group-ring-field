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

## graphicsGutz

PROJECT(GraphicsGutz)

###########################################

INCLUDE_DIRECTORIES(${GutzLib_SOURCE_DIR}/include)

###########################################

## Add in graphics gutz source

SET(GRAPHICS_GUTZ_SRC 
   Camera.cpp 
   Light.cpp 
   Manip.cpp)

## graphics gutz includes
SET(GRAPHICS_GUTZ_INC
   ${GutzLib_SOURCE_DIR}/include/graphicsGutz.h
   baseManip.h
   Camera.h
   gutzKeyMouse.h
   Light.h
   Manip.h
   vectorGraphics.h)

###########################################

ADD_LIBRARY(graphicsGutz ${GRAPHICS_GUTZ_SRC} ${GRAPHICS_GUTZ_INC})

##
## Hacked in by Pat to keep the Kong stuff a little cleaner... 
## 
IF(BUILD_KONG)
  ADD_DEFINITIONS(
    -DKONG
  )
ENDIF(BUILD_KONG)


