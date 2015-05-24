cmake_minimum_required(VERSION 2.8)

add_executable(testGutz         main.cpp)
add_executable(testDistrib      distribTest.cpp)
add_executable(testSignal       signalTest.cpp)
target_link_libraries(testSignal ${GUTZ_LIB})
add_executable(testThread       threadTest.cpp)
target_link_libraries(testThread ${GUTZ_LIB})
add_executable(testSmartPtrCast smartPtrCast.cpp)
add_executable(testVarCmdArgs   varCmdArgs.cpp) #  varCmdArgs.h)
target_link_libraries(testVarCmdArgs ${GUTZ_LIB})
add_executable(testCondition    conditionTest.cpp)
target_link_libraries(testCondition ${GUTZ_LIB})
add_executable(testBarrier      barrierTest.cpp)
target_link_libraries(testBarrier ${GUTZ_LIB})
add_executable(testTypelist     typelistTest.cpp)


