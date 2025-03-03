# SildenRSS
Imports RSS feeds.
Written in C++, as well as a compiler in powershell. Clickable titles, opens the links in web browser.
Will add a few more news sites later (maybe)

Needs WinLibs to launch; winget install winlibs / https://winlibs.com/#download-release, https://github.com/brechtsanders/winlibs_mingw

Needs to be set as env. variable; [System.Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\PATH\TO\mingw64\bin", [System.EnvironmentVariableTarget]::Machine)
