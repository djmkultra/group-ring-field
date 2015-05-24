Welcome to gutz!  Written primarily by Dr. Joe Kniss (Dr. Kristi Potter helped too)  

Configure using cmake

1) make a build directory (in the gutz directory "mkdir build")
2) move into build directory
3) type "ccmake .." (or ccmake <path to gutz source directory>)
4) you will need to hit the 'c' key twice (configure), then the 'g' key (generate make files)
5) type "make"  
6) run the exe's in the test directory to make sure you have a good build
7) enjoy!  You should be able to link in the libgutzlib.a, and use the include files.