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
!define PRODUCT_VERSION "0.0.0.16"
!define PRODUCT_URL  "http://pykd.codeplex.com/"
!define PRODUCT_NAME_AND_VERSION "${PRODUCT_FULL_NAME} ${PRODUCT_ARCH} ${PRODUCT_VERSION}"

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

#------------------------------------------------------------------------------
# Variables
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Interface Settings
#------------------------------------------------------------------------------
!define MUI_ABORTWARNING

!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange.bmp"

!define MUI_WELCOMEPAGE_TITLE_3LINES
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"

!define MUI_DIRECTORYPAGE_TEXT_TOP "Please specify path to $\"winext$\" subfolder of Debugging Tools for Windows (${PRODUCT_ARCH})."
!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Debugging Tools for Windows (${PRODUCT_ARCH})\winext folder"

!define MUI_FINISHPAGE_TITLE_3LINES
!define MUI_FINISHPAGE_TEXT_LARGE
!define MUI_FINISHPAGE_TEXT "${PRODUCT_NAME_AND_VERSION} was successfully installed.$\n$\n\
                            Run WinDbg and type $\".load pykd.pyd$\" to start using plugin."

!define MUI_FINISHPAGE_NOAUTOCLOSE

#------------------------------------------------------------------------------
# Pages
#------------------------------------------------------------------------------
# Installer Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "pykd_license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

#------------------------------------------------------------------------------
# Languages (first is default language)
#------------------------------------------------------------------------------
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

#------------------------------------------------------------------------------
# Installer file version information
#------------------------------------------------------------------------------
VIProductVersion "${PRODUCT_VERSION}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "${PRODUCT_SHORT_NAME}"
VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "${PRODUCT_NAME_AND_VERSION}"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName" "${MANUFACTURER}"
#VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks" "${PRODUCT_NAME} is a trademark of ${MANUFACTURER} company"
VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© All rights reserved"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "${PRODUCT_SHORT_NAME} setup"
VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${PRODUCT_VERSION}"

#------------------------------------------------------------------------------
# Before first page displayed
#------------------------------------------------------------------------------
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
    
    # Get installation folder from registry if available
    !if ${PRODUCT_ARCH} == "x64"
    ${IfThen} ${RunningX64} ${|} SetRegView 64 ${|}
    !endif
    ReadRegStr $INSTDIR HKLM "Software\${PRODUCT_SHORT_NAME}" "InstallPath"
    !if ${PRODUCT_ARCH} == "x64"
    ${IfThen} ${RunningX64} ${|} SetRegView 32 ${|}
    !endif
FunctionEnd


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

Section "${PRODUCT_SHORT_NAME}" Section1
    DetailPrint "Check Microsoft Visual C++ 2005 SP1 (${PRODUCT_ARCH}) runtime library..."
    push 'msvcr80.dll'
    #push 'Microsoft.VC80.CRT,version="8.0.50727.4027",type="win32",processorArchitecture="${ARCH}",publicKeyToken="1fc8b3b9a1e18e3b"'
    push 'Microsoft.VC80.CRT,version="8.0.50727.4053",type="win32",processorArchitecture="${ARCH}",publicKeyToken="1fc8b3b9a1e18e3b"'
    call WinSxS_HasAssembly
    pop $R0
    
    ${If} $R0 == 1
        DetailPrint "Success"
    ${Else}
        DetailPrint "Not find"
        DetailPrint "Going to download and install it."
        
        MessageBox MB_YESNOCANCEL|MB_ICONQUESTION "${PRODUCT_SHORT_NAME} requires Microsoft Visual C++ 2005 SP1 (${PRODUCT_ARCH}) runtime library. Download and install it automatically?$\n\
                                             Press YES to allow.$\n\
                                             Press NO to continue without runtime.$\n\
                                             Press CANCEL to exit from installer." IDYES DownloadAndInstallRuntime IDNO ContinueNoRuntime
            DetailPrint "${PRODUCT_FULL_NAME} installation cancelled."
            Abort
        DownloadAndInstallRuntime:
            DetailPrint "Downloading Microsoft Visual C++ 2005 SP1 (${PRODUCT_ARCH}) runtime library..."
            inetc::get /CAPTION "${PRODUCT_SHORT_NAME}" /QUESTION "" /POPUP "" /TIMEOUT=30000 "${VC_RUNTIME_URL}" "$TEMP\vcredist_${PRODUCT_ARCH}.exe" /END
            Pop $0
            ${If} $0 == "OK" 
                DetailPrint "Successfully downloaded."
                DetailPrint "Installing Microsoft Visual C++ 2005 SP1 (${PRODUCT_ARCH}) runtime library..."
            	IfErrors ClearErrorFlag
                ClearErrorFlag:
                ExecWait "$TEMP\vcredist_${PRODUCT_ARCH}.exe"
                IfErrors RuntimeInstallFailed
                DetailPrint "Successfully installed."
            ${Else}
                RuntimeInstallFailed:
                DetailPrint "Operation failed. Installation will be continued without Visual C++ runtime."
                DetailPrint "Please download and install it manually from product download page:"
                DetailPrint "${VC_RUNTIME_URL}"
            ${EndIf}
        ContinueNoRuntime:
        
    ${EndIf}
    
    
    DetailPrint "Check Python ${PYTHON_VERSION} ${PRODUCT_ARCH}..."
    !if ${PRODUCT_ARCH} == "x64"
    ${IfThen} ${RunningX64} ${|} SetRegView 64 ${|}
    !endif
    ReadRegStr $R0 HKLM "Software\Python\PythonCore\${PYTHON_VERSION}\InstallPath" ""
    !if ${PRODUCT_ARCH} == "x64"
    ${IfThen} ${RunningX64} ${|} SetRegView 32 ${|}
    !endif
    
    ${If} $R0 == ""
        DetailPrint "Appropiate Python version was no not find in:"
        DetailPrint "HKLM\Software\Python\PythonCore\${PYTHON_VERSION}\InstallPath"
        DetailPrint "Going to download and install it."
        MessageBox MB_YESNOCANCEL|MB_ICONQUESTION "${PRODUCT_SHORT_NAME} requires Python ${PYTHON_VERSION}. Download and install it automatically?$\n\
                                             Press YES to allow.$\n\
                                             Press NO to continue without Python.$\n\
                                             Press CANCEL to exit from installer." IDYES DownloadAndInstallPython IDNO ContinueNoPython
            DetailPrint "${PRODUCT_FULL_NAME} installation cancelled."
            Abort
        DownloadAndInstallPython:
            DetailPrint "Downloading Python..."
            inetc::get /CAPTION "${PRODUCT_SHORT_NAME}" /QUESTION "" /POPUP "" /TIMEOUT=30000 "${PYTHON_URL_BASE}${PYTHON_INSTALLER}" "$TEMP\${PYTHON_INSTALLER}" /END
            Pop $0
            ${If} $0 == "OK" 
                DetailPrint "Successfully downloaded."
                DetailPrint "Installing Python..."
            	IfErrors ClearErrorFlag1
                ClearErrorFlag1:
                ExecWait '"msiexec" /i "$TEMP\${PYTHON_INSTALLER}"'
                IfErrors PythonInstallFailed
                DetailPrint "Successfully installed."
            ${Else}
                PythonInstallFailed:
                DetailPrint "Operation failed. Installation will be continued without Python."
                DetailPrint "Please download and install it manually:"
                DetailPrint "${PYTHON_URL_BASE}${PYTHON_INSTALLER}"
            ${EndIf}
        ContinueNoPython:
    ${Else}
        DetailPrint "Python ${PYTHON_VERSION} ${PRODUCT_ARCH} location: $R0"
    ${EndIf}

    
    # Set Section properties
    SetOverwrite on

    # CURRENT USER
    SetShellVarContext current

    # Set Section Files and Shortcuts
    SetOutPath "$INSTDIR"

    # Extract files
    !if ${PRODUCT_ARCH} == "x64"
        File "..\x64\Release\pykd.pyd"
    !else
        File "..\Release\pykd.pyd"
    !endif
SectionEnd

Section -FinishSection
    #Store installation folder
    !if ${PRODUCT_ARCH} == "x64"
    ${IfThen} ${RunningX64} ${|} SetRegView 64 ${|}
    !endif
    WriteRegStr HKLM "Software\${PRODUCT_SHORT_NAME}" "InstallPath" "$INSTDIR"
    !if ${PRODUCT_ARCH} == "x64"
    ${IfThen} ${RunningX64} ${|} SetRegView 32 ${|}
    !endif
SectionEnd
