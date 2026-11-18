#requires -Version 5.1
param(
    [string]$Target = "C:\JiYuTrainer-1.7.9.1"
)

$ErrorActionPreference = "Stop"
$log = Join-Path $env:TEMP "JiYuTrainer-reinstall.log"

function Write-Log([string]$text) {
    $line = "[{0}] {1}" -f (Get-Date -Format "yyyy-MM-dd HH:mm:ss"), $text
    Add-Content -LiteralPath $log -Value $line -Encoding UTF8
    Write-Host $text
}

$principal = New-Object Security.Principal.WindowsPrincipal(
    [Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "需要管理员权限，正在重新启动..."
    $args = @(
        "-NoProfile",
        "-ExecutionPolicy", "Bypass",
        "-File", "`"$PSCommandPath`"",
        "-Target", "`"$Target`""
    )
    Start-Process -FilePath "powershell.exe" -Verb RunAs -ArgumentList $args
    exit
}

Write-Log "=== clean reinstall start ==="
Write-Log "package: $PSScriptRoot"
Write-Log "target : $Target"

# 1. stop old instances
foreach ($name in @("JiYuTrainer", "JiYuTrainerUpdater", "JiYuTrainerUpdateHelper")) {
    Get-Process -Name $name -ErrorAction SilentlyContinue | ForEach-Object {
        Write-Log "stop process: $($_.ProcessName) pid=$($_.Id)"
        Stop-Process -Id $_.Id -Force -ErrorAction SilentlyContinue
    }
}
Start-Sleep -Seconds 1

# 2. remove old kernel-driver services so the clean install recreates them
foreach ($svc in @("JiYuTrainerDriver", "TDProcHook")) {
    & sc.exe stop $svc 2>$null | Out-Null
    & sc.exe delete $svc 2>$null | Out-Null
    Write-Log "reset service: $svc"
}

# 3. move the old folder aside; never delete user data automatically
if (Test-Path -LiteralPath $Target) {
    $old = $Target + ".old-" + (Get-Date -Format "yyyyMMdd-HHmmss")
    Write-Log "rename old folder: $Target -> $old"
    Rename-Item -LiteralPath $Target -NewName (Split-Path -Leaf $old) -Force
}

New-Item -ItemType Directory -Path $Target -Force | Out-Null

# 4. copy a clean set; no old ini/log is copied
Get-ChildItem -LiteralPath $PSScriptRoot -Force |
    Where-Object { $_.Name -ne "reinstall.ps1" } |
    Copy-Item -Destination $Target -Recurse -Force

# 5. unblock files and clean stale update temp files
Get-ChildItem -LiteralPath $Target -Recurse -File | Unblock-File -ErrorAction SilentlyContinue
Remove-Item -Path (Join-Path $env:TEMP "JiYuTrainerUpdate*") -Recurse -Force -ErrorAction SilentlyContinue

$exe = Join-Path $Target "JiYuTrainer.exe"
if (-not (Test-Path -LiteralPath $exe)) {
    throw "JiYuTrainer.exe missing in $Target"
}

Write-Log "start: $exe"
Start-Process -FilePath $exe -WorkingDirectory $Target -Verb RunAs
Write-Log "=== clean reinstall done ==="
Write-Host ""
Write-Host "安装完成：$Target"
Write-Host "旧目录已改名保留，可确认新版正常后手动删除。"
Write-Host "如果 UDP 仍不可用，请把 $Target\JiYuTrainer.log 发回。"
