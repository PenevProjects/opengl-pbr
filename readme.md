## to run ##
Download a release version and run the .exe.
*Needs to be running on a Dedicated GPU for the PBR shaders to work!  
If you are on a laptop and are only getting hard colors without reflections, please use a GPU to run the application.*

## to rebuild ##
1. Install `cmake > 3.10`
2. Open command terminal in root directory
```
    mkdir build
    cd build
    cmake ..
``` 
3. Set "glPBR" as start-up project.
4. Make sure you compile in "Debug" mode and target architecture is "Win32".
