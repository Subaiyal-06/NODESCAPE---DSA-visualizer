$ws = New-Object -ComObject WScript.Shell
$desktop = [Environment]::GetFolderPath('Desktop')
$exePath = (Get-Item "$PSScriptRoot\build\NodeScapeApp.exe").FullName
$shortcut = $ws.CreateShortcut("$desktop\NodeScape Studio.lnk")
$shortcut.TargetPath = $exePath
$shortcut.WorkingDirectory = (Get-Item "$PSScriptRoot").FullName
$shortcut.Description = "NodeScape - Native DSA Visualizer Studio"
$shortcut.Save()
Write-Host "Created desktop shortcut: $desktop\NodeScape Studio.lnk"
