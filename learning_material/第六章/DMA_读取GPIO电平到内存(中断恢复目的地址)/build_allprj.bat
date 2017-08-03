@echo off 
SET TOOL="C:\Program Files (x86)\IAR Systems\Embedded Workbench 6.0\common\bin"


::@for /r  %%c in (*.ewp ) do   >>DZ10_Debug.txt (%TOOL%\iarbuild "%%c" -make DZ10_Debug  -log errors) 
@for /r  %%c in (*.ewp ) do (%TOOL%\iarbuild "%%c" -make DZ10_Debug  -log errors) 
@for /r  %%c in (*.ewp ) do (%TOOL%\iarbuild "%%c" -make DZ10_Release  -log errors) 
@for /r  %%c in (*.ewp ) do (%TOOL%\iarbuild "%%c" -make FX15_Debug  -log errors) 
@for /r  %%c in (*.ewp ) do (%TOOL%\iarbuild "%%c" -make FX15_Release  -log errors) 
:: errors  warnings info all

pause

