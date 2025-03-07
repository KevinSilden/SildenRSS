# SildenRSS
Imports RSS feeds.
Written in C++, as well as some powershell to generate a .exe.

Tested on a physical machine running x64 W11 as well as a VM running x64 W10.

Clickable titles, opens the links in web browser when clicked. No change of colour when first clicked, yet. 
Scrolling is working, but it doesn't scroll back up when changing RSS feeds(yet). So a bit b0rked.
Will add a few more news sites later (maybe).
Sets colours based on system(registry).

<strike>Needs WinLibs to launch; winget install winlibs / https://winlibs.com/#download-release, https://github.com/brechtsanders/winlibs_mingw

Needs to be set as env. variable; [System.Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\PATH\TO\mingw64\bin", [System.EnvironmentVariableTarget]::Machine)

Command "g++ --version" to check version on system. </strike>

PowerShell updated, now statically linking MINGW64 in the .exe

Had some good help using a certain LLM on this one.. :)

![image](https://github.com/user-attachments/assets/4a1708e5-e44e-45b0-92bb-b1e538c283b7)
![image](https://github.com/user-attachments/assets/37adef65-d33d-4334-9834-103d234f3edc)
![image](https://github.com/user-attachments/assets/a9740069-8749-4b01-b8e5-3acd69cc3e07)

