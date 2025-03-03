# SildenRSS
Imports RSS feeds.
Written in C++, as well as a compiler in powershell. Clickable titles, opens the "links"(blue titles) in web browser when clicked.
Will add a few more news sites later (maybe).
Sets colours based on system, light mode is atrocious atm.

Needs WinLibs to launch; winget install winlibs / https://winlibs.com/#download-release, https://github.com/brechtsanders/winlibs_mingw

Needs to be set as env. variable; [System.Environment]::SetEnvironmentVariable("Path", $env:Path + ";C:\PATH\TO\mingw64\bin", [System.EnvironmentVariableTarget]::Machine)

![image](https://github.com/user-attachments/assets/4a1708e5-e44e-45b0-92bb-b1e538c283b7)
![image](https://github.com/user-attachments/assets/2f5933a3-38fe-4560-8a74-9788df63ef11)
![image](https://github.com/user-attachments/assets/661136f5-c61d-4b41-9cbd-5c73835878b6)
