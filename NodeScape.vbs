' NodeScape Silent App Launcher
' Launches the visualizer desktop window without flashing a command prompt

Set objFSO = CreateObject("Scripting.FileSystemObject")
Set objShell = CreateObject("WScript.Shell")

scriptDir = objFSO.GetParentFolderName(WScript.ScriptFullName)
batPath = scriptDir & "\NodeScape-App.bat"

objShell.Run """" & batPath & """", 0, False
Set objShell = Nothing
Set objFSO = Nothing
