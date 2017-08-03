@echo off 
SET TOOL="C:\Program Files (x86)\WANdisco\Subversion"

copy .\Board\src\VCAN_MK60_conf.c .\App\MK60_conf.c
copy .\Board\inc\VCAN_MK60_conf.h .\App\inc\MK60_conf.h
copy .\Board\inc\VCAN_PORT_cfg.h .\App\inc\PORT_cfg.h

echo %~dp0

:: 添加文件（没提交）
%TOOL%\svn add "%~dp0\App\MK60_conf.c"
%TOOL%\svn add "%~dp0\App\inc\MK60_conf.h"
%TOOL%\svn add "%~dp0\App\inc\PORT_cfg.h"
%TOOL%\svn del "%~dp0\Board\src\VCAN_MK60_conf.c"
%TOOL%\svn del "%~dp0\Board\inc\VCAN_MK60_conf.h"
%TOOL%\svn del "%~dp0\Board\inc\VCAN_PORT_cfg.h"

pause