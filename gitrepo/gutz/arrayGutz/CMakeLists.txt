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

## ArrayGutz
##  Right now this is not really a library, since there is 
##  nothing to compile, but this cmake file will create a 
##  MSVC project so we can hack on the code.

PROJECT(ArrayGutz)

###########################################

SET(ARRAY_GUTZ_INC
   ${GutzLib_SOURCE_DIR}/include/arrayGutz.h
   arrayBase.h
   arrayOwn1.h
   arrayOwn2.h
   arrayOwn3.h
   arrayOwn4.h
   arrayOwn5.h
   arrayWrap1.h
   arrayWrap2.h
   arrayWrap3.h
   arrayWrap4.h
   arrayWrap5.h
)

###########################################
## Create Library (Project really)
ADD_LIBRARY(ArrayGutz ${ARRAY_GUTZ_INC})