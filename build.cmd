@echo off
echo 开始构建项目...

REM 检查Visual Studio 2022社区版
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" (
    echo 找到Visual Studio 2022社区版
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    goto Build
)

REM 检查Visual Studio 2022专业版
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat" (
    echo 找到Visual Studio 2022专业版
    call "C:\Program Files\Microsoft Visual Studio\2022\Professional\Common7\Tools\VsDevCmd.bat"
    goto Build
)

REM 检查Visual Studio 2022企业版
if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat" (
    echo 找到Visual Studio 2022企业版
    call "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\Common7\Tools\VsDevCmd.bat"
    goto Build
)

REM 检查Visual Studio 2019社区版
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" (
    echo 找到Visual Studio 2019社区版
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"
    goto Build
)

echo 未找到支持的Visual Studio版本，编译失败
goto End

:Build
echo 设置Visual Studio环境完成

if not exist build mkdir build
cd build
cmake ..
if %errorlevel% neq 0 (
    echo CMake配置失败
    goto End
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo 构建失败
    goto End
)

echo 构建完成！

:End
pause 