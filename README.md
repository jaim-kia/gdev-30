This package contains (almost) everything you need to start making OpenGL programs. You just need the following additional things:

1. A C++ compiler. For Windows, we shall use the MinGW-w64 gcc compiler. For macOS, we shall use the clang compiler. Instructions for installing these are provided in this README.md file.

2. A text editor. I recommend that you use Visual Studio Code (although you can use any editor you prefer). If you do install VS Code, I recommend installing the following extensions (click View->Extensions and search for these in the Marketplace):
   - ms-vscode.cpptools (C/C++ IntelliSense)
   - slevesque.shader (Shader languages support for VS Code)

Happy hacking! - eric

*******************************************************************************

Instructions for Windows:
-------------------------

1. Download and install the MinGW-w64 gcc compiler.

   - I recommend that you download precompiled binaries from https://github.com/niXman/mingw-builds-binaries/releases/download/14.2.0-rt_v12-rev1/x86_64-14.2.0-release-posix-seh-ucrt-rt_v12-rev1.7z . Otherwise, you can figure out how to build it yourself from https://www.mingw-w64.org/ (not recommended for this class).

   - Extract the contents of the .7z file (using a tool like 7-Zip, available from https://www.7-zip.org/) to your C:\ drive.

   - If you did it correctly, there should be a mingw64 folder (the contents of the .7z) directly on your C:\ drive.

2. Add the folder C:\mingw64\bin to your system's environment variables.

   - Open the Start menu, type the word "env", then click "Edit environment variables for your account".

     - If you instead clicked "Edit the system environment variables": On the window that appears, click on the "Environment Variables..." button near the bottom-right.

   - On the upper pane of the Environment Variables window, double-click the variable "Path".

   - Click "New", then type in "C:\mingw64\bin" (without the quotes).

   - Click OK to confirm the addition.

   - Click OK (on the Environment Variables window) to save the environment variables.

3. Open a Command Prompt or Windows Powershell window. (From the Start menu, type the word "cmd" or "powershell", and click the relevant item that appears.)

4. Confirm that your MinGW-w64 compiler is working by entering the command:

       g++

   The command should give you the message:

       g++: fatal error: no input files
       compilation terminated.

5. Change to the folder where you extracted this GDEVGL.zip. For example, if you extracted it to your desktop as-is, type the command:

       cd Desktop\GDEVGL

6. Finally, to try out one of the example programs, run the test.cmd script provided. For example, to test out the first demo, type:

       .\test demo0.cpp

   The test script will helpfully display the full command line that you would actually type in to manually compile your program. (Aren't you glad that we can automate this?) The command line may also serve as a guide for when you make your own "makefiles" to build larger applications.

*******************************************************************************

Instructions for macOS:
-----------------------

1. Open a Terminal window. (Click the Launchpad icon on the Dock, then click "Other", then "Terminal".)

2. Confirm that you have the command line developer tools installed by entering the command:

       clang++

   - If you have not used clang or Xcode before, this will prompt you to install the command line developer tools. Click "Install", and wait for a couple of hours (depending on your connection speed).

   - If it is already installed, the command should give you the message:

         clang: error: no input files

3. Change to the folder where you extracted this GDEVGL.zip. For example, if you extracted it to your desktop as-is, type the command:

       cd ~/Desktop/GDEVGL

4. A testing script is provided for you (so that you don't have to type lengthy compilation commands when you test your OpenGL programs), but it will not work out-of-the-box. Type this command to make it work (you only need to do this once):

       chmod +x ./test

5. Finally, to try out one of the example programs, run the testing script. For example, to test out the first demo, type:

       ./test demo0.cpp

   The test script will helpfully display the full command line that you would actually type in to manually compile your program. (Aren't you glad that we can automate this?) The command line may also serve as a guide for when you make your own "makefiles" to build larger applications.

*******************************************************************************

This package contains the following third-party software:
---------------------------------------------------------

1. GLAD version 0.1.36 (https://glad.dav1d.de/)

   - Automatically-generated header and library-loader files for OpenGL (since the ones that ship with Windows and macOS are cumbersome to use).

   - The GLAD in this package is generated for OpenGL version 3.3 core (and I added all supported extensions); also, the glad.c file is renamed to glad.cpp to avoid compilation warnings, since we will be using C++.

2. GLFW version 3.4 (https://www.glfw.org/download.html)

   - A cross-platform wrapper library for OpenGL that provides auxiliary OS functions such as window system management and input handling.

   - Only the header files and the Windows (lib-mingw-w64) and macOS (lib-universal) precompiled library binaries are included in this package.

3. GLM version 1.0.1 (https://github.com/g-truc/glm/releases)

   - A C++ header-only library for math operations, closely following GLSL naming conventions and functionality.

   - Only the header files are included in this package.

4. stb_image.h version 2.30 (https://github.com/nothings/stb/blob/master/stb_image.h)

   - A header-only library for loading images.
