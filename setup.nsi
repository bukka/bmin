# Bmin NSIS script
# 21.7.2010 16:09:30

Name Bmin

RequestExecutionLevel admin

# General Symbol Definitions
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 1.0.1
!define COMPANY "Jakub Zelenka"
!define URL http://bukka.eu

# Bmin Symbol Definicition
!define QTLIBDIR "C:\Qt\2010.02.1\qt\bin"
!define UNNAME "$(^Name) Uninstall"

# MUI Symbol Definitions
!define MUI_COMPONENTSPAGE_NODESC
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_REGISTRY_ROOT HKLM
!define MUI_STARTMENUPAGE_REGISTRY_KEY ${REGKEY}
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME StartMenuGroup
!define MUI_STARTMENUPAGE_DEFAULTFOLDER $(^Name)
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN $INSTDIR\bmin.exe
!define MUI_UNFINISHPAGE_NOAUTOCLOSE
!define MUI_ICON "qtgui\data\icon.ico"
!define MUI_UNICON "qtgui\data\icon.ico"

# Included files
!include Sections.nsh
!include MUI2.nsh

# Variables
Var StartMenuGroup

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE license.txt
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuGroup
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile bmin-${VERSION}-win32.exe
InstallDir $PROGRAMFILES\Bmin
CRCCheck on
XPStyle on
ShowInstDetails hide
VIProductVersion ${VERSION}.0
VIAddVersionKey ProductName $(^Name)
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription ""
VIAddVersionKey LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
ShowUninstDetails hide

# Installer sections
Section "!Bmin executables" SEC_BMIN
    SectionIn RO
    SetOutPath $INSTDIR
    SetOverwrite on
    File "${QTLIBDIR}\mingwm10.dll"
    File "${QTLIBDIR}\libgcc_s_dw2-1.dll"
    File "${QTLIBDIR}\QtCore4.dll"
    File "${QTLIBDIR}\QtGui4.dll"
    File "${QTLIBDIR}\QtOpenGL4.dll"
    File "qtgui\data\icon.ico"
    File "license.txt"
    File "release\bmin.exe"
    WriteRegStr HKLM "${REGKEY}\Components" Bmin 1
SectionEnd

Section "Create icon on the Desktop" SEC_DESKTOP
    CreateShortcut "$DESKTOP\$(^Name).lnk" "$INSTDIR\bmin.exe" "" "$INSTDIR\icon.ico" "0" SW_SHOWMAXIMIZED
    WriteRegStr HKLM "${REGKEY}\Components" Desktop 1
SectionEnd

Section "Add Bmin to the Start Menu" SEC_START
    #CreateDirectory "$SMPROGRAMS\$StartMenuGroup"  
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    SetOutPath $SMPROGRAMS\$StartMenuGroup
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\${UNNAME}.lnk" "$INSTDIR\$(^Name) Uninstall.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuGroup\$(^Name).lnk" "$INSTDIR\bmin.exe" "" "$INSTDIR\icon.ico" "0" SW_SHOWNORMAL
    !insertmacro MUI_STARTMENU_WRITE_END
    WriteRegStr HKLM "${REGKEY}\Components" Start 1
SectionEnd

Section -post SEC_POST
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller "$INSTDIR\${UNNAME}.exe"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon "$INSTDIR\${UNNAME}.exe"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString "$INSTDIR\${UNNAME}.exe"
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Bmin UNSEC_BMIN
    Delete /REBOOTOK $INSTDIR\bmin.exe
    Delete /REBOOTOK $INSTDIR\license.txt
    Delete /REBOOTOK $INSTDIR\QtOpenGL4.dll
    Delete /REBOOTOK $INSTDIR\QtGui4.dll
    Delete /REBOOTOK $INSTDIR\QtCore4.dll
    Delete /REBOOTOK $INSTDIR\libgcc_s_dw2-1.dll
    Delete /REBOOTOK $INSTDIR\mingwm10.dll
    DeleteRegValue HKLM "${REGKEY}\Components" Bmin
SectionEnd

Section /o -un.Desktop UNSEC_DESKTOP
    Delete /REBOOTOK "$DESKTOP\$(^Name).lnk"
    DeleteRegValue HKLM "${REGKEY}\Components" Desktop
SectionEnd

Section /o -un.Start UNSEC_START
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\${UNNAME}.lnk"
    Delete /REBOOTOK "$SMPROGRAMS\$StartMenuGroup\$(^Name).lnk"
    RmDir /REBOOTOK $SMPROGRAMS\$StartMenuGroup
    DeleteRegValue HKLM "${REGKEY}\Components" Start
    DeleteRegValue HKLM "${REGKEY}" StartMenuGroup
SectionEnd

Section -un.post UNSEC_POST
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK "$INSTDIR\$(^Name) Uninstall.exe"
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
    RmDir /REBOOTOK $INSTDIR
SectionEnd

# Installer functions
Function .onInit
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuGroup
    !insertmacro SELECT_UNSECTION Bmin ${UNSEC_BMIN}
    !insertmacro SELECT_UNSECTION Desktop ${UNSEC_DESKTOP}
    !insertmacro SELECT_UNSECTION Start ${UNSEC_START}
FunctionEnd

