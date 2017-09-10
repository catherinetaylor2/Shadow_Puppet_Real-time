# Shadow Puppet Real-time

Using OpenGL and summed area tables to produce virtual shadow puppets in real-time as part of my EngD dissertation.

![gitimages](https://user-images.githubusercontent.com/25514442/29090356-3a4a63ae-7c77-11e7-910b-283dbf2e6e5d.png)

<p align="center">
  <img width="400" height="300" src="https://user-images.githubusercontent.com/25514442/29090152-8973a18a-7c76-11e7-9958-47183abe176f.gif">
  </p>
  
  
### Input:
 * quad for puppet position
 * bitmap for puppet texture
 * bitmap for screen texture
 
### Method:
This software uses:
  * rectangular light source
  * diffuse lighting
  * soft shadows
  * integral images
  
### Output:
This software can produce:
  * still images (HD frame rate ~ 260fps)
  * animations (HD frame rate ~ 52fps)

### Dependencies:

To run successfully, this code must be linked to the following 4 libraries.

* OpenGl
* GLEW 
* GLFW3
* GLM

### How to compile:

CL:

     cl /O2 /EHsc  main.cpp shader.cpp BITMAP.cpp ReadObj.cpp openGLCalculations.cpp /link /NODEFAULTLIB:"LIBCMT"  "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\glfw3.lib" "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\lib\glew32.lib" "C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86\OpenGL32.Lib"  "user32.lib" "gdi32.lib"  "shell32.lib" "msvcrt.lib" 

### To run in terminal:
 
 main xres yres
