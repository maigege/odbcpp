;NSIS odbcpp installer script, complete with license.
;Written by Alexis Wilke
;Copyright (c) 2008 Made to Order Software Corporation

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

	;Name and file
	Name "Made to Order Software\odbcpp"
	;AddBrandingImage top 67 0
	BrandingText /trimcenter "Made to Order Software Corp."
	OutFile "odbcpp-1.6-win32.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\Made to Order Software Corp\odbcpp"

	;Get installation folder from registry if available
	InstallDirRegKey HKCU "Software\Made to Order Software Corp\odbcpp" ""

	;Request application privileges for Windows Vista
	RequestExecutionLevel admin

;--------------------------------
;Variables
	Var StartMenuFolder

;--------------------------------
;Interface Configuration

 	!define MUI_HEADERIMAGE
 	!define MUI_HEADERIMAGE_BITMAP "odbcpp-install.bmp" ; optional
 	!define MUI_ABORTWARNING

;--------------------------------
;Pages

 	!insertmacro MUI_PAGE_WELCOME
 	!insertmacro MUI_PAGE_LICENSE "COPYING.txt"
 	!insertmacro MUI_PAGE_COMPONENTS
 	!insertmacro MUI_PAGE_DIRECTORY
  	;
	;Start Menu Folder Page Configuration
	;
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Made to Order Software Corp\odbcpp" 
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
	;
 	!insertmacro MUI_PAGE_INSTFILES
 	;!insertmacro MUI_PAGE_FINISH

 	!insertmacro MUI_UNPAGE_WELCOME
 	!insertmacro MUI_UNPAGE_CONFIRM
 	!insertmacro MUI_UNPAGE_INSTFILES
 	;!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "odbcpp library" SecDeveloper

	SetOutPath "$INSTDIR\include\odbcpp"
	File "..\include\odbcpp\*.h"

	SetOutPath "$INSTDIR\src"
	File "..\src\*.cpp"

	SetOutPath "$INSTDIR\tests"
	File "..\tests\*.cpp"

	SetOutPath "$INSTDIR\win32"
	File "*.vcproj"
	File "*.sln"
	File "README.txt"

	SetOutPath "$INSTDIR\lib"
	File "lib\*.lib"

	SetOutPath "$INSTDIR\doc"
	File "COPYING.txt"
	File "README.txt"

	;Store installation folder
	WriteRegStr HKCU "Software\Made to Order Software Corp\odbcpp" "" $INSTDIR

	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	;Create shortcuts
	;
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Read the odbcpp License.lnk" "$INSTDIR\doc\COPYING.txt"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Read the README.txt file.lnk" "$INSTDIR\doc\README.txt"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "documentation" SecDocumentation

	SetOutPath "$INSTDIR\doc\html"
	File "..\doc\html\*.*"

	;Create shortcuts
	;
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Documentation.lnk" "$INSTDIR\doc\html\index.html"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_SecDeveloper ${LANG_ENGLISH} "Installs the odbcpp Sources, Headers, Libraries and Tests."
	LangString DESC_SecDocumentation ${LANG_ENGLISH} "Installs odbcpp Library Documentation."

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDeveloper} $(DESC_SecDeveloper)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecDocumentation} $(DESC_SecDocumentation)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

	;Delete "$INSTDIR\bin\*.*"
	RMDir /r "$INSTDIR"

	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	;
	Delete "$SMPROGRAMS\$StartMenuFolder\Read the odbcpp License.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Read the README.txt file.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Documentation.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"

	DeleteRegKey /ifempty HKCU "Software\Made to Order Software Corp\odbcpp"

SectionEnd

; vim: ts=4 sw=4
