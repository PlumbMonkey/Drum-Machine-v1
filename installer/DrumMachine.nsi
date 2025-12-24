; Drum Machine v0.1.0 - Windows NSIS Installer
; For use with Nullsoft Scriptable Install System (NSIS)
; Download NSIS: https://nsis.sourceforge.io/

!include "MUI2.nsh"

; Basic Settings
Name "Drum Machine"
OutFile "DrumMachine-0.1.0-Windows.exe"
InstallDir "$PROGRAMFILES\DrumMachine"
InstallDirRegKey HKLM "Software\DrumMachine" "Install_Dir"

; Require admin for installation
RequestExecutionLevel admin

; MUI Settings
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

; Version Info
VIProductVersion "0.1.0.0"
VIAddVersionKey "ProductName" "Drum Machine"
VIAddVersionKey "ProductVersion" "0.1.0"
VIAddVersionKey "CompanyName" "Drum Machine"
VIAddVersionKey "FileDescription" "Step Sequencer and Sample Player"
VIAddVersionKey "FileVersion" "0.1.0"
VIAddVersionKey "LegalCopyright" "MIT License"

; Build from directory where NSIS is run
; Expected: run from d:\Dev Projects 2025\Drum Machine-v1\installer\
; Sources: ..\build\bin\Release\ and ..\assets

; Installer Section
Section "Install"
    SetOutPath "$INSTDIR"
    
    ; Copy executable and DLLs
    File "D:\Dev Projects 2025\Drum Machine-v1\build\bin\Release\DrumMachine.exe"
    File "D:\Dev Projects 2025\Drum Machine-v1\build\bin\Release\SDL2.dll"
    File "D:\Dev Projects 2025\Drum Machine-v1\build\bin\Release\rtaudio.dll"
    File "D:\Dev Projects 2025\Drum Machine-v1\build\bin\Release\rtmidi.dll"
    
    ; Copy assets and samples
    SetOutPath "$INSTDIR\assets\samples"
    File /nonfatal "D:\Dev Projects 2025\Drum Machine-v1\assets\samples\*.*"
    
    ; Create directories for user content
    CreateDirectory "$INSTDIR\patterns"
    CreateDirectory "$INSTDIR\midi"
    CreateDirectory "$INSTDIR\samples"
    
    ; Create Start Menu shortcuts
    SetShellVarContext all
    CreateDirectory "$SMPROGRAMS\Drum Machine"
    CreateShortcut "$SMPROGRAMS\Drum Machine\Drum Machine.lnk" "$INSTDIR\DrumMachine.exe"
    CreateShortcut "$SMPROGRAMS\Drum Machine\Uninstall.lnk" "$INSTDIR\uninstall.exe"
    
    ; Create Desktop shortcut
    CreateShortcut "$DESKTOP\Drum Machine.lnk" "$INSTDIR\DrumMachine.exe"
    
    ; Write registry for uninstall
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumMachine" "DisplayName" "Drum Machine"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumMachine" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumMachine" "InstallLocation" "$INSTDIR"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumMachine" "DisplayVersion" "0.1.0"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"
    
    ; Store install directory
    WriteRegStr HKLM "Software\DrumMachine" "Install_Dir" "$INSTDIR"
SectionEnd

; Uninstaller Section
Section "Uninstall"
    SetShellVarContext all
    
    ; Remove Start Menu shortcuts
    RMDir /r "$SMPROGRAMS\Drum Machine"
    
    ; Remove Desktop shortcut
    Delete "$DESKTOP\Drum Machine.lnk"
    
    ; Remove installed files (keep user patterns/MIDI)
    Delete "$INSTDIR\DrumMachine.exe"
    Delete "$INSTDIR\SDL2.dll"
    Delete "$INSTDIR\rtaudio.dll"
    Delete "$INSTDIR\rtmidi.dll"
    Delete "$INSTDIR\uninstall.exe"
    
    ; Remove empty directories
    RMDir /r "$INSTDIR\assets"
    RMDir "$INSTDIR"
    
    ; Remove registry entries
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DrumMachine"
    DeleteRegKey HKLM "Software\DrumMachine"
SectionEnd

; Function to display finish page
Function .onInstSuccess
    MessageBox MB_OK "Drum Machine has been installed successfully!$\n$\nLook for the shortcut on your Desktop."
FunctionEnd
