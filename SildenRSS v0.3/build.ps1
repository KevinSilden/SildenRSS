# Ensure the directories exists, if not, create them
if (!(Test-Path "build")) {
    mkdir "build"
}

if (!(Test-Path "build/debug")) {
    mkdir "build/debug"
}

if (!(Test-Path "build/modules")) {
    mkdir "build/modules"
}

if (!(Test-Path "build/resources")) {
    mkdir "build/resources"
}

if (!(Test-Path "build/resources/libraries")) {
    mkdir "build/resources/libraries"
}

if (Test-Path "src/resources/resources.rc") {
    windres src/resources/resources.rc -O coff -o build/resources.res
} else {
    Write-Host "Error: src/resources/resources.rc not found!" -ForegroundColor Red
    exit 1
}

if (!(Test-Path "build/resources.res")) {
    Write-Host "Error: resources.res was not generated!" -ForegroundColor Red
    exit 1
}

# Compile source files
$sourceFiles = @(
    "src/main.cpp",
    "src/modules/window.cpp",
    "src/resources/libraries/pugixml.cpp"
)


$objectFiles = $sourceFiles -replace "src/", "build/" -replace "\.cpp$", ".o"

foreach ($source in $sourceFiles) {
    $object = $source -replace "src/", "build/" -replace "\.cpp$", ".o"
    g++ -c $source -o $object -I src/modules/headers -mwindows
    if ($LASTEXITCODE -ne 0) {
        Write-Host "Compilation failed for $source" -ForegroundColor Red
        exit 1
    }
}

# Link all object files and resources.res into the final executable, MINGW64 now statically linked, tested on a VM
g++ -I "src/modules/headers" -I "src/resources/libraries" -o build/SildenRSS.exe `
    $objectFiles build/resources.res -mwindows -static -static-libgcc -static-libstdc++ -ldwmapi -lwininet

if (!(Test-Path "build/SildenRSS.exe")) {
    Write-Host "Error: SildenRSS.exe was not created!" -ForegroundColor Red
    exit 1
}

Write-Host "Build complete! Executable is in build/SildenRSS.exe"
