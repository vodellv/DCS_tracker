:: Build script for AppVeyor (https://ci.appveyor.com/)
:: Environment:
::     TOOLCHAIN      -  Toolchain Version  [9.0/10.0/11.0/12.0/14.0/mingw]
::     CONFIGURATION  -  determines EPICS build  [dynamic/static, -debug]
::     PLATFORM       -  "x86" -> use 32bit architecture
::
:: Prepares an Appveyor build by excuting the following steps
::     - Set up configure\CONFIG_SITE for static vs. dynamic build
::     - Install Mingw  (TOOLCHAIN setting) in the in the appropriate flavor
::     - Download and install Make-4.1 from EPICS download page

Setlocal EnableDelayedExpansion

set OS=64BIT
if "%PLATFORM%"=="x86" set OS=32BIT

echo [INFO] Platform: %OS%

if "%TOOLCHAIN%"=="mingw" (
    echo.%CONFIGURATION% | findstr /C:"static">nul && (
        echo SHARED_LIBRARIES=NO>> configure\CONFIG_SITE
        echo STATIC_BUILD=YES>> configure\CONFIG_SITE
        echo [INFO] EPICS set up for static build
    ) || (
        echo [INFO] EPICS set up for dynamic build
    )
    echo.%CONFIGURATION% | findstr /C:"debug">nul && (
        echo HOST_OPT=NO>> configure\CONFIG_SITE
        echo [INFO] EPICS set up for debug build
    ) || (
        echo [INFO] EPICS set up for optimized build
    )
)

echo [INFO] Installing Make 4.2.1 from ANL web site
curl -fsS --retry 3 -o C:\tools\make-4.2.1.zip https://epics.anl.gov/download/tools/make-4.2.1-win64.zip
cd \tools
"C:\Program Files\7-Zip\7z" e make-4.2.1.zip
