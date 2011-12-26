#
# This is setup script for pykd project.
# Author: Maksim K. aka airmax
#

SetCompressor LZMA

# Modern interface settings
!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "WinVer.nsh"
!include "FileFunc.nsh"
!include "x64.nsh"
!include "Sections.nsh"

!addincludedir .\Include
!addplugindir .\Plugins

!include "HasAssembly.nsh"

#------------------------------------------------------------------------------
# General
#------------------------------------------------------------------------------

!define PYTHON_VERSION "2.6"
!define PYTHON_URL_BASE "http://python.org/ftp/python/2.6.6/"

!define PRODUCT_SHORT_NAME "pykd"
!define PRODUCT_FULL_NAME  "Python extension for WinDbg"
!define PRODUCT_VERSION "0.0.1.2"
!define PRODUCT_URL  "http://pykd.codeplex.com/"
!define PRODUCT_NAME_AND_VERSION "${PRODUCT_FULL_NAME} ${PRODUCT_ARCH} ${PRODUCT_VERSION}"
!define PRODUCT_MANUFACTURER "PyKd Team"

!if ${PRODUCT_ARCH} == "x64"
    !define ARCH "amd64"
!else
    !define ARCH "x86"
!endif

# Main Install settings
Name "${PRODUCT_NAME_AND_VERSION}"
OutFile "${PRODUCT_SHORT_NAME}_${PRODUCT_ARCH}_${PRODUCT_VERSION}_setup.exe"

BrandingText "${PRODUCT_FULL_NAME}"

#Request application privileges for Windows Vista and 7
RequestExecutionLevel admin

ShowInstDetails show
ShowUninstDetails show

#------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Interface Settings
#------------------------------------------------------------------------------
!define MUI_ABORTWARNING

!define MUI_ICON "MUIOrangeVistaTheme\Clean\installer.ico"
!define MUI_UNICON "MUIOrangeVistaTheme\Clean\uninstaller.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "MUIOrangeVistaTheme\Clean\header-l.bmp"
!define MUI_HEADERIMAGE_UNBITMAP "MUIOrangeVistaTheme\Clean\header-l-un.bmp"

!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEFINISHPAGE_BITMAP "MUIOrangeVistaTheme\Clean\wizard.bmp"

!define MUI_DIRECTORYPAGE_TEXT_TOP "Please specify path to $\"winext$\" subfolder of Debugging Tools for Windows (${PRODUCT_ARCH})."
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Debugging Tools for Windows (${PRODUCT_ARCH})\winext folder"

!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT_LARGE
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME_AND_VERSION} was successfully installed.$\n$\n\
                            Run WinDbg and type $\".load pykd.pyd$\" to start using it."

!define MUI_FINISHPAGE_NOAUTOCLOSE

!define MUI_COMPONENTSPAGE_SMALLDESC

#------------------------------------------------------------------------------
# Pages
#------------------------------------------------------------------------------
# Installer Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "pykd_license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

# Uninstaller Pages
#!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES

#------------------------------------------------------------------------------
# Languages (first is default language)
#------------------------------------------------------------------------------
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

#------------------------------------------------------------------------------
# Installer file version information
#------------------------------------------------------------------------------
VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName"     "${PRODUCT_SHORT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments"        "${PRODUCT_NAME_AND_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName"     "${PRODUCT_MANUFACTURER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "${PRODUCT_SHORT_NAME} is a trademark of ${PRODUCT_MANUFACTURER}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright"  "© All rights reserved 2010-2012"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_SHORT_NAME} setup"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion"     "${PRODUCT_VERSION}"

#------------------------------------------------------------------------------
# Some logic-lib like macro
#------------------------------------------------------------------------------
!macro SetRegView64
    !if ${PRODUCT_ARCH} == "x64"
    SetRegView 64
    !endif
!macroend
!define SetRegView64 "!insertmacro SetRegView64"

!macro SetRegView32
    !if ${PRODUCT_ARCH} == "x64"
    SetRegView 32
    !endif
!macroend
!define SetRegView32 "!insertmacro SetRegView32"

!macro _IsVcRuntimeInstalled _a _b _t _f
    !insertmacro _LOGICLIB_TEMP
    Push 'msvcr90.dll'
    Push 'Microsoft.VC90.CRT,version="9.0.21022.8",type="win32",processorArchitecture="${ARCH}",publicKeyToken="1fc8b3b9a1e18e3b"'
    ${WinSxS_HasAssembly}   
    Pop $_LOGICLIB_TEMP
    !insertmacro _== $_LOGICLIB_TEMP 1 `${_t}` `${_f}`
!macroend
!define IsVcRuntimeInstalled `"" IsVcRuntimeInstalled ""`
!define un.IsVcRuntimeInstalled `"" IsVcRuntimeInstalled ""`

!macro _IsPythonInstalled _a _b _t _f
    !insertmacro _LOGICLIB_TEMP
    ${SetRegView64}
    # Check Python ${PYTHON_VERSION} ${PRODUCT_ARCH}...
    ReadRegStr $_LOGICLIB_TEMP HKLM "Software\Python\PythonCore\${PYTHON_VERSION}\InstallPath" ""
    ${SetRegView32}
    !insertmacro _!= $_LOGICLIB_TEMP "" `${_t}` `${_f}`
!macroend
!define IsPythonInstalled `"" IsPythonInstalled ""`
!define un.IsPythonInstalled `"" IsPythonInstalled ""`

!macro _IsDiaRegistered _a _b _t _f
    !insertmacro _LOGICLIB_TEMP
    ${SetRegView64}
    ReadRegStr $_LOGICLIB_TEMP HKCR "CLSID\${CLSID_DiaSource}\InprocServer32" ""
    ${SetRegView32}
    !insertmacro _!= $_LOGICLIB_TEMP "" `${_t}` `${_f}`
!macroend
!define IsDiaRegistered `"" IsDiaRegistered ""`
!define un.IsDiaRegistered `"" IsDiaRegistered ""`
        
#------------------------------------------------------------------------------
# Check WinDbg executable presence in selected directory
#------------------------------------------------------------------------------
Function .onVerifyInstDir
    ${IfNotThen} ${FileExists} "$INSTDIR\..\WinDbg.exe"  ${|} Abort ${|}

    ${GetFileName} "$INSTDIR" $R0
    ${IfThen} $R0 != "WinExt" ${|} Abort ${|}
FunctionEnd

#------------------------------------------------------------------------------
# Installer Sections
#------------------------------------------------------------------------------

Section "${PRODUCT_SHORT_NAME} ${PRODUCT_ARCH}" sec_pykd
    SectionIn RO
    
    # Set Section properties
    SetOverwrite on

    # CURRENT USER
    SetShellVarContext current

    DetailPrint "Extracting extension..."
    SetOutPath "$INSTDIR"

    !if ${PRODUCT_ARCH} == "x64"
        File "..\x64\Release\pykd.pyd"
    !else
        File "..\Release\pykd.pyd"
    !endif
SectionEnd

Section "Snippets" sec_snippets
    # Set Section properties
    SetOverwrite on

    DetailPrint "Extracting snippets..."
    SetOutPath "$DOCUMENTS\${PRODUCT_SHORT_NAME}"
    File "..\Snippets\*.py"
SectionEnd

Section "Python ${PYTHON_VERSION} ${PRODUCT_ARCH}" sec_python
    DetailPrint "Downloading Python..."
    inetc::get /CAPTION "${PRODUCT_SHORT_NAME}" /QUESTION "" /POPUP "" /TIMEOUT=30000 "${PYTHON_URL_BASE}${PYTHON_INSTALLER}" "$TEMP\${PYTHON_INSTALLER}" /END
    Pop $0
    ${If} $0 == "OK" 
        DetailPrint "Successfully downloaded."
        DetailPrint "Installing Python..."
        ClearErrors
        ExecWait '"msiexec" /i "$TEMP\${PYTHON_INSTALLER}"'
        IfErrors PythonInstallFailed
        DetailPrint "Successfully installed."
        ${SetRegView64}
        WriteRegDWORD HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallPython" 1
        ${SetRegView32}
    ${Else}
        PythonInstallFailed:
        DetailPrint "Operation failed. Installation will be continued without Python."
        DetailPrint "Please download and install it manually:"
        DetailPrint "${PYTHON_URL_BASE}${PYTHON_INSTALLER}"
    ${EndIf}
SectionEnd

Section "Visual C++ 2008 SP1 (${PRODUCT_ARCH}) runtime" sec_vcruntime
    DetailPrint "Installing Microsoft Visual C++ 2008 SP1 (${PRODUCT_ARCH}) runtime library..."

    SetOutPath "$TEMP"
    !if ${PRODUCT_ARCH} == "x64"
        File "..\x64\Release\vcredist_${PRODUCT_ARCH}.exe"
    !else
        File "..\Release\vcredist_${PRODUCT_ARCH}.exe"
    !endif
    
    ExecWait "$TEMP\vcredist_${PRODUCT_ARCH}.exe"
    ${IfNot} ${Errors}
        DetailPrint "Successfully installed."
        ${SetRegView64}
        WriteRegDWORD HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallVcRuntime" 1
        ${SetRegView32}
    ${Else}
        DetailPrint "Operation failed. Installation will be continued without Visual C++ runtime."
        DetailPrint "Please download and install it manually."
    ${EndIf}
SectionEnd

Section "Debug Interface Access (${PRODUCT_ARCH}) library" sec_msdia
    DetailPrint "Registering Debug Interface Access (${PRODUCT_ARCH}) library..."
    !if ${PRODUCT_ARCH} == "x64"
        RegDLL "$COMMONFILES64\Microsoft Shared\VC\${MSDIA_DLL_NAME}"
    !else
        RegDLL "$COMMONFILES32\Microsoft Shared\VC\${MSDIA_DLL_NAME}"
    !endif
    ${IfNot} ${Errors}
        DetailPrint "Successfully registered."
        ${SetRegView64}
        WriteRegDWORD HKLM "Software\${PRODUCT_SHORT_NAME}" "UnregisterDia" 1
        ${SetRegView32}
    ${Else}
        DetailPrint "Operation failed. Please register it manually."
    ${EndIf}
SectionEnd

Section -FinishSection
    ${SetRegView64}
    DetailPrint "Storing installation folder..."
    WriteRegStr HKLM "Software\${PRODUCT_SHORT_NAME}" "InstallPath" "$INSTDIR"
    
    DetailPrint "Adding extension dir and snippets dir to PYTHONPATH..."
    WriteRegStr HKLM "Software\Python\PythonCore\${PYTHON_VERSION}\PythonPath\${PRODUCT_SHORT_NAME}" "" "$INSTDIR;$DOCUMENTS\${PRODUCT_SHORT_NAME}"
    
    DetailPrint "Registering uninstaller..."
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}" "DisplayName" "${PRODUCT_FULL_NAME} (${PRODUCT_ARCH})"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}" "UninstallString" "$LOCALAPPDATA\${PRODUCT_SHORT_NAME}\uninstall_${PRODUCT_ARCH}.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}" "Publisher" "${PRODUCT_MANUFACTURER}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}" "URLInfoAbout" "${PRODUCT_URL}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}" "DisplayVersion" "${PRODUCT_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}" "DisplayIcon" "$LOCALAPPDATA\${PRODUCT_SHORT_NAME}\uninstall_${PRODUCT_ARCH}.exe"
    ${SetRegView32}
    
    DetailPrint "Writing uninstaller..."
    CreateDirectory "$LOCALAPPDATA\${PRODUCT_SHORT_NAME}"
    WriteUninstaller "$LOCALAPPDATA\${PRODUCT_SHORT_NAME}\uninstall_${PRODUCT_ARCH}.exe"
SectionEnd

#------------------------------------------------------------------------------
# Before first page displayed
#------------------------------------------------------------------------------

# Workaround to allow ${IsVcRuntimeInstalled} work properly
!define WinSxS_HasAssembly `Call WinSxS_HasAssembly`

Function .onInit
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_SHORT_NAME}_${PRODUCT_ARCH}_setup") i .r1 ?e'
    Pop $R0
    ${If} $R0 != 0
        MessageBox MB_OK|MB_ICONEXCLAMATION "The installer is already running."
        Abort
    ${EndIf}

    !if ${PRODUCT_ARCH} == "x64"
    ${IfNot} ${RunningX64}
        MessageBox MB_OK|MB_ICONEXCLAMATION "This installation requires 64-bit OS."
        Abort
    ${EndIf}
    !endif
    
    ${SetRegView64}
    # Get installation folder from registry if available
    ReadRegStr $INSTDIR HKLM "Software\${PRODUCT_SHORT_NAME}" "InstallPath"
    ${SetRegView32}
    
    ${If} ${IsPythonInstalled}
        !insertmacro UnselectSection ${sec_python}
        SectionSetText ${sec_python} ""
    ${EndIf}
    
    ${If} ${IsVcRuntimeInstalled}
        !insertmacro UnselectSection ${sec_vcruntime}
        SectionSetText ${sec_vcruntime} ""
    ${EndIf}
    
    ${If} ${IsDiaRegistered}
        !insertmacro UnselectSection ${sec_msdia}
        SectionSetText ${sec_msdia} ""
    ${EndIf}
FunctionEnd

#------------------------------------------------------------------------------
# Installer sections descriptions
#------------------------------------------------------------------------------

LangString DESC_sec_pykd      ${LANG_ENGLISH} "${PRODUCT_FULL_NAME}"
LangString DESC_sec_snippets  ${LANG_ENGLISH} "Useful code snippets. Will be installed in $DOCUMENTS\${PRODUCT_SHORT_NAME}"
LangString DESC_sec_python    ${LANG_ENGLISH} "Let installer download and setup Python ${PYTHON_VERSION} ${PRODUCT_ARCH}"
LangString DESC_sec_vcruntime ${LANG_ENGLISH} "Let installer download and setup Microsoft Visual C++ 2008 SP1 (${PRODUCT_ARCH}) runtime library"
LangString DESC_sec_msdia     ${LANG_ENGLISH} "Let installer register Debug Interface Access (${PRODUCT_ARCH}) library"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${sec_pykd}      $(DESC_sec_pykd)
    !insertmacro MUI_DESCRIPTION_TEXT ${sec_snippets}  $(DESC_sec_snippets)
    !insertmacro MUI_DESCRIPTION_TEXT ${sec_python}    $(DESC_sec_python)
    !insertmacro MUI_DESCRIPTION_TEXT ${sec_vcruntime} $(DESC_sec_vcruntime)
    !insertmacro MUI_DESCRIPTION_TEXT ${sec_msdia}     $(DESC_sec_msdia)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

#------------------------------------------------------------------------------
# Uninstaller sections
#------------------------------------------------------------------------------
Section "un.${PRODUCT_SHORT_NAME} ${PRODUCT_ARCH}" unsec_pykd
    SectionIn RO

    ${SetRegView64}
    DetailPrint "Retriving installation folder from registry..."
    ReadRegStr $INSTDIR HKLM "Software\${PRODUCT_SHORT_NAME}" "InstallPath"

    DetailPrint "Deleting extension dir and snippets dir from PYTHONPATH..."
    DeleteRegKey HKLM "Software\Python\PythonCore\${PYTHON_VERSION}\PythonPath\${PRODUCT_SHORT_NAME}"
    
    DetailPrint "Unregistering uninstaller..."
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_SHORT_NAME}"
    ${SetRegView32}

    DetailPrint "Deleting extension..."
    Delete "$INSTDIR\pykd.pyd"
    
    # Let user delete snippets himself
    #RMDir "$DOCUMENTS\${PRODUCT_SHORT_NAME}"
    
    DetailPrint "Deleting uninstaller..."
    Delete "$LOCALAPPDATA\${PRODUCT_SHORT_NAME}\uninstall_${PRODUCT_ARCH}.exe"
    RMDir "$LOCALAPPDATA\${PRODUCT_SHORT_NAME}"
SectionEnd

Section /o "un.Python ${PYTHON_VERSION} ${PRODUCT_ARCH}" unsec_python
    DetailPrint "Uninstalling Python..."
    ExecWait '"msiexec" /x ${PYTHON_PRODUCT_CODE}'
SectionEnd

Section /o "un.Debug Interface Access (${PRODUCT_ARCH}) library" unsec_msdia
    DetailPrint "Unregistering Debug Interface Access (${PRODUCT_ARCH}) library..."
    ${SetRegView64}
    ReadRegStr $R0 HKCR "CLSID\${CLSID_DiaSource}\InprocServer32" ""
    ${SetRegView32}
    UnRegDLL $R0
SectionEnd

Section /o "un.Visual C++ 2008 SP1 (${PRODUCT_ARCH}) runtime" unsec_vcruntime
    DetailPrint "Uninstalling Microsoft Visual C++ 2008 SP1 (${PRODUCT_ARCH}) runtime library..."
    ExecWait '"msiexec" /x ${VCRUNTIME_PRODUCT_CODE}'
SectionEnd

Section -un.FinishSection
    ${SetRegView64}
    WriteRegDWORD HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallPython" 0
    WriteRegDWORD HKLM "Software\${PRODUCT_SHORT_NAME}" "UnregisterDia" 0
    WriteRegDWORD HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallVcRuntime" 0
#    DeleteRegValue HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallPython"
#    DeleteRegValue HKLM "Software\${PRODUCT_SHORT_NAME}" "UnregisterDia"
#    DeleteRegValue HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallVcRuntime"
    ${SetRegView32}
SectionEnd

#------------------------------------------------------------------------------
# Before first page displayed
#------------------------------------------------------------------------------

# Workaround to allow ${IsVcRuntimeInstalled} work properly
!undef WinSxS_HasAssembly
!define WinSxS_HasAssembly `Call un.WinSxS_HasAssembly`

Function un.onInit
    ${SetRegView64}
    ReadRegDWORD $R0 HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallPython"
    ${SetRegView32}

    ${If} ${Errors}
    ${OrIf} $R0 == 0
    ${OrIfNot} ${IsPythonInstalled}
        !insertmacro UnselectSection ${unsec_python}
        SectionSetText ${unsec_python} ""
    ${EndIf}

    ${SetRegView64}
    ReadRegDWORD $R0 HKLM "Software\${PRODUCT_SHORT_NAME}" "UnregisterDia"
    ${SetRegView32}
    
    ${If} ${Errors}
    ${OrIf} $R0 == 0
    ${OrIfNot} ${IsDiaRegistered}
        !insertmacro UnselectSection ${unsec_msdia}
        SectionSetText ${unsec_msdia} ""
    ${EndIf}
    
    ${SetRegView64}
    ReadRegDWORD $R0 HKLM "Software\${PRODUCT_SHORT_NAME}" "UninstallVcRuntime"
    ${SetRegView32}

    ${If} ${Errors}
    ${OrIf} $R0 == 0
    ${OrIfNot} ${IsVcRuntimeInstalled}
        !insertmacro UnselectSection ${unsec_vcruntime}
        SectionSetText ${unsec_vcruntime} ""
    ${EndIf}
FunctionEnd

#------------------------------------------------------------------------------
# Uninstaller sections descriptions
#------------------------------------------------------------------------------

LangString DESC_unsec_pykd      ${LANG_ENGLISH} "${PRODUCT_FULL_NAME}"
LangString DESC_unsec_python    ${LANG_ENGLISH} "Uninstall Python ${PYTHON_VERSION} ${PRODUCT_ARCH}"
LangString DESC_unsec_msdia     ${LANG_ENGLISH} "Unregister Debug Interface Access (${PRODUCT_ARCH}) library"
LangString DESC_unsec_vcruntime ${LANG_ENGLISH} "Uninstall Microsoft Visual C++ 2008 SP1 (${PRODUCT_ARCH}) runtime library"

!insertmacro MUI_UNFUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${unsec_pykd}      $(DESC_unsec_pykd)
    !insertmacro MUI_DESCRIPTION_TEXT ${unsec_python}    $(DESC_unsec_python)
    !insertmacro MUI_DESCRIPTION_TEXT ${unsec_msdia}     $(DESC_unsec_msdia)
    !insertmacro MUI_DESCRIPTION_TEXT ${unsec_vcruntime} $(DESC_unsec_vcruntime)
!insertmacro MUI_UNFUNCTION_DESCRIPTION_END
