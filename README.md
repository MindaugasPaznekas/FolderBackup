 ### Overview
- This is CLI program for Windows which backups files from one directory to another. It accepts hot folder path and backup folder path, backing-up any file that is created or modified in the chosen folder.  

### Features
- creates a copy of any file created or modified in the hot folder   
- backup files have the same name of the original file with .bak extension   
- if the file name is prefixed with 'delete_' it will be immediately deleted from the hot folder and backup folder      
- keeps a log file of all action taken by your program (file created, altered, backedup or deleted)   
- log file can be viewed/filtered by you CLI app.   
- log file filters accepts filter by [date, text, filename regex]   
- the application will work between reboots updating only changed files in provided directories 

### How to build it

1. Download or clone the project.

2. Install cmake, add it to Your path. (I used version 3.25)

3.  Build and Compile.

    Option A: MSVC compiler
    Use MSVC compiler built in Visual Studio (I used Community version)
    When installing or upgrading VS select workload 'Desktop Development with C++' and make sure MSVC v143 is used(newest).
    Open the VS project and build (Ctrl + B).
    Note: You may need to turn on C++20 in Project->FolderBackup properties->C/C++->Language->C++ Language standart


    Option B: MingW compiler
    Follow these guides to install MingW and MSYS
    https://code.visualstudio.com/docs/languages/cpp#_install-a-compiler
    I used version: https://github.com/msys2/msys2-installer/releases/download/2023-01-27/msys2-x86_64-20230127.exe

    Then run these commands:
    mkdir build
    cd build
    cmake .. -G "MinGW Makefiles"
    mingw32-make

4.  Finally run the built executable. Provide it with hot and backup folder locations. 
    Hot folder must exist, backup can exist or will be created automatically.
    FolderBackup.exe C:\hot C:\backup