@echo off
setlocal

cl /MD /c ^
 /I"D:\opengl\Dependencies\GLEW\include" ^
 /I"D:\opengl\Dependencies\GLFW\include" ^
 Main.cpp


link Main.obj ^
 /LIBPATH:"D:\opengl\Dependencies\GLEW\lib\Release\x64" ^
 /LIBPATH:"D:\opengl\Dependencies\GLFW\lib-vc2022" ^
 glew32.lib glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib kernel32.lib ^
 /SUBSYSTEM:CONSOLE ^
 /OUT:Main.exe