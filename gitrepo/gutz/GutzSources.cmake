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

## GUTZ SOURCES, used by both FindGutz and gutz/Cmakelists.txt
#
#  This cmake find file will define the following variables
# GUTZ_DEFINES
# GUTZ_PATH 
# GUTZ_INCLUDE_PATH
# GUTZ_INCLUDE_FILES
# GUTZ_LIB_PATH           !!!! NOT DEFINED !!!!
# GUTZ_LIB
# GUTZ_SOURCE_FILES
#
#  This file also defines the individial files for each sub-lib
# MATH_GUTZ_INC
#
#

IF( NOT GUTZ_SOURCED )
SET( GUTZ_SOURCED )

SET(GUTZ_LIB "gutzlib")

SET(GUTZ_DEFINES "-D USING_MATHGUTZ")

###############################################################
## Include directories
###############################################################

IF(NOT GUTZ_LIB_BUILD)
  SET(GUTZ_PATH "gutz/" 
      CACHE PATH 
      "Path to root GUTZ directory"
  )
ENDIF(NOT GUTZ_LIB_BUILD)

SET(GUTZ_INCLUDE_PATH "${GUTZ_PATH}/include")


###############################################################
## FILE Variables
###############################################################

SET(GUTZ_INCLUDE_FILES "")
SET(GUTZ_SOURCE_FILES "")

###############################################################
## Various GUTZ
###############################################################

SET(GUTZ_INCLUDE_FILES "${GUTZ_INCLUDE_PATH}/timeGutz.h" 
			${GUTZ_INCLUDE_FILES}
   )

###############################################################
## MATH GUTZ
###############################################################

## Mathgutz directory
SET(MATH_GUTZ_PATH "${GUTZ_PATH}/mathGutz")
SET(MGD ${MATH_GUTZ_PATH})

## Mathgutz src
SET(MATH_GUTZ_INC
   include/mathGutz.h
   ${MGD}/mat.h
   ${MGD}/mat2.h
   ${MGD}/mat3.h
   ${MGD}/mat4.h
   ${MGD}/vec.h
   ${MGD}/quat.h
   ${MGD}/ray2.h
   ${MGD}/ray3.h
   ${MGD}/plane.h
   ${MGD}/vecConst.h
   ${MGD}/mathExt.h
   ${MGD}/mm.h
   ${MGD}/vecIter.h
   ${MGD}/vecMath.h
   ${MGD}/distribBase.h
   ${MGD}/gaussDistrib.h
   ${MGD}/asymmGaussDistrib.h	
   ${MGD}/uniformDistrib.h	
   ${MGD}/chiDistrib.h		
   ${MGD}/chiSquareDistrib.h		
   ${MGD}/exponentialDistrib.h		
   ${MGD}/laplaceDistrib.h		
   ${MGD}/asymmLaplaceDistrib.h			
   ${MGD}/poissonDistrib.h		
   ${MGD}/rayleighDistrib.h		
   ${MGD}/riceDistrib.h	
   ${MGD}/weibullDistrib.h	
   ${MGD}/multiDistrib.h	
   include/distribGutz.h
)

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${MATH_GUTZ_INC})
SET(MATH_GUTZ_SRC ${MATH_GUTZ_INC})

SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${MATH_GUTZ_PATH})

###############################################################
## Array GUTZ
###############################################################

## Arraygutz directory
SET(ARRAYGUTZ_PATH "${GUTZ_PATH}/arrayGutz")
SET(AGD ${ARRAYGUTZ_PATH})

## Arraygutz src
SET(ARRAY_GUTZ_INC
   include/arrayGutz.h
   ${AGD}/arrayBase.h
   ${AGD}/arrayOwn1.h
   ${AGD}/arrayOwn2.h
   ${AGD}/arrayOwn3.h
   ${AGD}/arrayOwn4.h
   ${AGD}/arrayOwn5.h
   ${AGD}/arrayWrap1.h
   ${AGD}/arrayWrap2.h
   ${AGD}/arrayWrap3.h
   ${AGD}/arrayWrap4.h
   ${AGD}/arrayWrap5.h
)

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${ARRAY_GUTZ_INC})
SET(ARRAY_GUTZ_SRC ${ARRAY_GUTZ_INC})

SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${ARRAYGUTZ_PATH})

###############################################################
## Graphics GUTZ
###############################################################

# Graphicsgutz directory
SET(GRAPHICSGUTZ_PATH "${GUTZ_PATH}/graphicsGutz")
SET(GGD ${GRAPHICSGUTZ_PATH})

# Graphicsgutz src
SET(GRAPHICS_GUTZ_INC 
   include/graphicsGutz.h
   ${GGD}/baseManip.h   
   ${GGD}/Camera.h
   ${GGD}/Light.h
   ${GGD}/Manip.h
   ${GGD}/ManipEvents.h
   ${GGD}/ManipEventBase.h
   ${GGD}/vectorGraphics.h
)

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${GRAPHICS_GUTZ_INC})


SET(GRAPHICS_GUTZ_CPP 
   ${GGD}/baseManip.cpp
   ${GGD}/Camera.cpp 
   ${GGD}/Light.cpp 
   ${GGD}/Manip.cpp
   ${GGD}/ManipEvents.cpp
)

SET(GUTZ_SOURCE_FILES ${GUTZ_SOURCE_FILES} ${GRAPHICS_GUTZ_CPP})

SET(GRAPHICS_GUTZ_SRC ${GRAPHICS_GUTZ_INC} ${GRAPHICS_GUTZ_CPP})

SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${GRAPHICSGUTZ_PATH})


###############################################################
## File Gutz
###############################################################
SET(FILEGUTZ_INC
	include/fileGutz.h
)

SET(FILEGUTZ_PATH "${GUTZ_PATH}/fileGutz")

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${FILEGUTZ_INC})
SET(FILEGUTZ_CPP ${FILEGUTZ_PATH}/fileGutz.cpp)
SET(GUTZ_SOURCE_FILES ${GUTZ_SOURCE_FILES} ${FILEGUTZ_CPP})

###############################################################
## Smart Pointers
###############################################################

SET(SMARTPTR_INC
	include/smartptr.h
)

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${SMARTPTR_INC})
SET(SMARTPTR_SRC ${SMARTPTR_INC})

###############################################################
## signals & slots
###############################################################

SET(SIGNALGUTZ_PATH "${GUTZ_PATH}/signalGutz")
SET(SSDR ${SIGNALGUTZ_PATH})

SET(SIGSLOT_INC
	include/signalGutz.h
	${SSDR}/signalCall.h
	${SSDR}/g_signal.h
        ${SSDR}/signalIF.h
        ${SSDR}/command.h
)
SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${SIGSLOT_INC})

SET(SIGSLOT_CPP
	${SSDR}/signalSlots.cpp
)
SET(GUTZ_SOURCE_FILES ${GUTZ_SOURCE_FILES} ${SIGSLOT_CPP})

SET(SIGSLOT_SRC ${SIGSLOT_INC} ${SIGSLOT_CPP})

SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${SIGNALGUTZ_PATH})

###############################################################
## events 
###############################################################

SET(EVENTGUTZ_PATH "${GUTZ_PATH}/eventGutz")
SET(EDR ${EVENTGUTZ_PATH})
SET(EVENT_GUTZ_INC
	include/eventGutz.h
	${EDR}/cameraEvent.h
        ${EDR}/deviceEvent.h
        ${EDR}/deviceState.h
	${EDR}/gutzKeyMouse.h
	${EDR}/idleEvent.h
	${EDR}/keyEvent.h
	${EDR}/mouseEvent.h
	${EDR}/renderEvent.h
)
SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${EVENT_GUTZ_INC})

SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${EVENTGUTZ_PATH})

###############################################################
## threads 
###############################################################

SET(THREADGUTZ_PATH "${GUTZ_PATH}/threadGutz")
SET(TDR ${THREADGUTZ_PATH})
SET(THREAD_GUTZ_INC
	include/threadGutz.h
	${TDR}/thread.h
	${TDR}/nothread.h
	${TDR}/threadPolicy.h
	${TDR}/mutexPolicy.h
	${TDR}/conditionPolicy.h
	${TDR}/threadWorker.h
	${TDR}/threadWorker.cpp
)

IF(APPLE OR UNIX)
	SET(THREAD_GUTZ_INC
		${THREAD_GUTZ_INC}
		${TDR}/pthreadGutz.h
		${TDR}/pthreadGutz.cpp
		${TDR}/qthreadGutz.h   # Needs to be included on Mac, Windows and Linux
	)
ELSE(APPLE OR UNIX)
	SET(THREAD_GUTZ_INC
		${THREAD_GUTZ_INC}
		${TDR}/qthreadGutz.h
	)
ENDIF(APPLE OR UNIX)

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${THREAD_GUTZ_INC})
SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${THREADGUTZ_PATH})

###############################################################
## exceptions 
###############################################################

SET(EXCEPTIONGUTZ_PATH "${GUTZ_PATH}/exceptionGutz")
SET(EXDR ${EXCEPTIONGUTZ_PATH})
SET(EXCEPTION_GUTZ_INC
	include/exceptionGutz.h
	${EXDR}/exception.h
)
SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${EXCEPTION_GUTZ_INC})
SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${EXCEPTIONGUTZ_PATH})

###############################################################
## Simulation
###############################################################
SET(SIM_GUTZ_PATH "${GUTZ_PATH}/simGutz")
SET(SIM_GUTZ_INC
	include/simGutz.h
	${SIM_GUTZ_PATH}/Field.h
	${SIM_GUTZ_PATH}/Integrator.h
	${SIM_GUTZ_PATH}/Particle.h
	${SIM_GUTZ_PATH}/RK4.h
	${SIM_GUTZ_PATH}/Simulation.h
)

SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES} ${SIM_GUTZ_INC})
SET(GUTZ_INCLUDE_PATH ${GUTZ_INCLUDE_PATH} ${SIM_GUTZ_PATH})

# Misc includes
SET(GUTZ_INCLUDE_FILES ${GUTZ_INCLUDE_FILES}
  include/docuverse.h
  include/typelistGutz.h
  include/templateTraits.h
  include/staticCheckGutz.h
  include/phdlib.h
  include/storagePolicyGutz.h
  include/typeLambda.h
  include/unitGutz.h
)

ENDIF( NOT GUTZ_SOURCED )
