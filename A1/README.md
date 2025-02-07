This program is controlled using the mouse on a ImGui window. 
Click/hold and drag the bars to change shape, iteration values, etc. 
The values for bonus points will show up when on the tree shape. 
You may have to scroll down on the ImGui window to see them.

If for whatever reason ImGui fail to run, uncomment lines 282, 283 in main.cpp and run the program using callbacks.
The callbacks are controled via arrow keys on the keyboard:
	Left/Right to change the shape, Up/Down to change iteration.
Bonus points will not work if using the callbacks 

OS: Windows 11 Home V24H2, build 26100.2894
Compiler: Microsoft (R) C/C++ Optimizing Compiler Version 19.29.30158 for x86

Running the program:
Navigate to the root folder then in the terminal
```mkdir build```
```cmake ..```
```cmake --build . --config Release```
```./Release/453-skeleton.exe```
