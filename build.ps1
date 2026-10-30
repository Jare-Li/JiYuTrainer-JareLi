#requires -Version 5.1
<#
    JiYuTrainer 1.7.6 - Jare-Li Refactor build script.
    Usage:
        pwsh -File .\build.ps1 -Sign
        pwsh -File .\build.ps1 -Sign -SkipTests
#>
param(
    [switch]$Sign,
    [switch]$SkipTests,
    [switch]$Clean
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'

$RepoRoot = $PSScriptRoot
$Src = $RepoRoot
$Release = Join-Path $Src 'Release'
$Build = Join-Path $Src 'build'
$Dist = Join-Path $RepoRoot 'dist'
$LogDir = Join-Path $Build 'logs'

if (-not (Test-Path $Src)) { throw "Source directory not found: $Src" }
New-Item -ItemType Directory -Force $Build, $LogDir, $Dist | Out-Null

# ---------------------------------------------------------------- toolchain
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if (-not (Test-Path $vswhere)) { throw 'vswhere.exe not found. Install Visual Studio 2022 with C++ workload.' }
$VsPath = (& $vswhere -latest -products * -requires Microsoft.Component.MSBuild -property installationPath).Trim()
if (-not $VsPath) { throw 'Visual Studio with MSBuild was not found.' }
$MSBuild = Join-Path $VsPath 'MSBuild\Current\Bin\MSBuild.exe'
if (-not (Test-Path $MSBuild)) { throw "MSBuild not found: $MSBuild" }

$Msvc = Get-ChildItem (Join-Path $VsPath 'VC\Tools\MSVC') -Directory |
    Sort-Object { [version]$_.Name } -Descending | Select-Object -First 1
if (-not $Msvc) { throw 'MSVC toolset not found.' }

$SdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10'
$SdkVersion = Get-ChildItem (Join-Path $SdkRoot 'Include') -Directory |
    Where-Object { $_.Name -match '^10\.' } |
    Sort-Object { [version]$_.Name } -Descending | Select-Object -First 1
if (-not $SdkVersion) { throw 'Windows 10/11 SDK not found.' }
$SdkVer = $SdkVersion.Name

$LibExe = Join-Path $Msvc.FullName 'bin\Hostx64\x86\lib.exe'
$ClExe = Join-Path $Msvc.FullName 'bin\Hostx64\x86\cl.exe'

Write-Host "== Toolchain ==" -ForegroundColor Cyan
Write-Host "MSVC       : $($Msvc.Name)"
Write-Host "Windows SDK: $SdkVer"
Write-Host "MSBuild    : $MSBuild"

$commonArgs = @(
    '/m', '/nologo', '/v:minimal',
    '/p:Configuration=Release',
    '/p:Platform=Win32',
    '/p:PlatformToolset=v143',
    "/p:SolutionDir=$Src\",
    "/p:WindowsTargetPlatformVersion=$SdkVer"
)

function Invoke-MSBuild([string]$project, [string]$label, [switch]$Rebuild) {
    Write-Host "== Build $label ==" -ForegroundColor Cyan
    $log = Join-Path $LogDir ("{0}.log" -f $label)
    $msbuildArgs = @($project) + $commonArgs
    if ($Rebuild) { $msbuildArgs += '/t:Rebuild' }
    & $MSBuild @msbuildArgs 2>&1 | Tee-Object -FilePath $log | Select-Object -Last 25
    if ($LASTEXITCODE -ne 0) { throw "Build failed: $label (see $log)" }
}

# ---------------------------------------------------------------- Sciter SDK
$SciterSdk = Join-Path $Src 'JiYuTrainerUI\sciter-sdk'
$SciterLibDir = Join-Path $SciterSdk 'lib\32'
$SciterLib = Join-Path $SciterLibDir 'sciter.lib'
$SciterDll = Join-Path $SciterLibDir 'sciter.dll'
if (-not (Test-Path $SciterDll)) {
    Write-Host '== Prepare Sciter SDK ==' -ForegroundColor Cyan
    New-Item -ItemType Directory -Force $SciterLibDir | Out-Null
    Expand-Archive -Path (Join-Path $Release 'sciter.zip') -DestinationPath $SciterLibDir -Force
}
if (-not (Test-Path $SciterLib)) {
    Write-Host '== Generate sciter.lib ==' -ForegroundColor Cyan
    $def = Join-Path $SciterLibDir 'sciter.def'
    @(
        'LIBRARY sciter.dll',
        'EXPORTS',
        '    SciterAPI'
    ) | Set-Content -Encoding ASCII $def
    & $LibExe "/def:$def" /machine:x86 "/out:$SciterLib"
    if ($LASTEXITCODE -ne 0) { throw 'Failed to generate sciter.lib' }
}

# ---------------------------------------------------------------- build order
if ($Clean) {
    Write-Host '== Clean Release outputs ==' -ForegroundColor Cyan
    Get-ChildItem $Release -File | Where-Object {
        $_.Name -in @('JiYuTrainer.exe', 'JiYuTrainerHooks.dll', 'JiYuTrainerHooks.lib',
                      'JiYuTrainerHooks.exp', 'JiYuTrainerUI.lib', 'JiYuTrainerUpdater.lib')
    } | Remove-Item -Force
}

Invoke-MSBuild (Join-Path $Src 'JiYuTrainerUpdater\JiYuTrainerUpdater.vcxproj') 'updater'
Invoke-MSBuild (Join-Path $Src 'JiYuTrainerUI\JiYuTrainerUI.vcxproj') 'ui'
Invoke-MSBuild (Join-Path $Src 'JiYuTrainerHooks\JiYuTrainerHooks.vcxproj') 'hooks'

# ---------------------------------------------------------------- signing
$SignTool = $null
$Pfx = Join-Path $Build 'certs\Jare-Li.pfx'
$PwdFile = Join-Path $Build 'certs\password.txt'
if ($Sign) {
    Write-Host '== Code signing certificate ==' -ForegroundColor Cyan
    New-Item -ItemType Directory -Force (Split-Path $Pfx) | Out-Null
    if (-not (Test-Path $Pfx)) {
        $password = [Guid]::NewGuid().ToString('N')
        [IO.File]::WriteAllText($PwdFile, $password, [Text.Encoding]::ASCII)

        # Create the self-signed code-signing certificate in memory. This avoids
        # writing to Cert:\CurrentUser\My, which can be blocked in restricted
        # build environments.
        $rsa = [System.Security.Cryptography.RSA]::Create(2048)
        $request = [System.Security.Cryptography.X509Certificates.CertificateRequest]::new(
            'CN=Jare-Li',
            $rsa,
            [System.Security.Cryptography.HashAlgorithmName]::SHA256,
            [System.Security.Cryptography.RSASignaturePadding]::Pkcs1)
        $request.CertificateExtensions.Add(
            [System.Security.Cryptography.X509Certificates.X509BasicConstraintsExtension]::new($false, $false, 0, $false))
        $request.CertificateExtensions.Add(
            [System.Security.Cryptography.X509Certificates.X509KeyUsageExtension]::new(
                [System.Security.Cryptography.X509Certificates.X509KeyUsageFlags]::DigitalSignature, $true))
        $oids = [System.Security.Cryptography.OidCollection]::new()
        $null = $oids.Add([System.Security.Cryptography.Oid]::new('1.3.6.1.5.5.7.3.3'))
        $request.CertificateExtensions.Add(
            [System.Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension]::new($oids, $false))
        $cert = $request.CreateSelfSigned(
            [DateTimeOffset]::Now.AddDays(-1),
            [DateTimeOffset]::Now.AddYears(10))
        [IO.File]::WriteAllBytes($Pfx, $cert.Export(
            [System.Security.Cryptography.X509Certificates.X509ContentType]::Pfx, $password))
        [IO.File]::WriteAllBytes(
            (Join-Path $Build 'certs\Jare-Li.cer'),
            $cert.Export([System.Security.Cryptography.X509Certificates.X509ContentType]::Cert))
        Write-Host "Created self-signed code-signing certificate CN=Jare-Li"
    }
    if (-not (Test-Path $PwdFile)) { throw "Missing PFX password file: $PwdFile" }
    $password = (Get-Content -Raw $PwdFile).Trim()

    if (-not (Test-Path $PwdFile)) { throw "Missing PFX password file: $PwdFile" }
    $password = (Get-Content -Raw $PwdFile).Trim()

    # SignTool imports the PFX into the user certificate store, which can be
    # blocked in restricted build environments. Set-AuthenticodeSignature with
    # an ephemeral key does the same Authenticode signing entirely in memory.
    function Invoke-SignFile([string]$file) {
        $cert = New-Object System.Security.Cryptography.X509Certificates.X509Certificate2(
            $Pfx, $password, [System.Security.Cryptography.X509Certificates.X509KeyStorageFlags]::EphemeralKeySet)
        $result = Set-AuthenticodeSignature -FilePath $file -Certificate $cert -HashAlgorithm SHA256
        if (-not $result.SignerCertificate -or $result.SignerCertificate.Subject -notlike '*CN=Jare-Li*') {
            throw "Failed to sign $file : $($result.Status) $($result.StatusMessage)"
        }
        Write-Host "Signed $([IO.Path]::GetFileName($file)) : $($result.SignerCertificate.Subject) ($($result.Status))"
    }

    Write-Host '== Sign hook DLL (before embedding) ==' -ForegroundColor Cyan
    Invoke-SignFile (Join-Path $Release 'JiYuTrainerHooks.dll')
}

# Main exe embeds JiYuTrainerHooks.dll; build it after signing the hook.
Invoke-MSBuild (Join-Path $Src 'JiYuTrainer\JiYuTrainer.vcxproj') 'main' -Rebuild:$Sign

if ($Sign) {
    Write-Host '== Sign main executable ==' -ForegroundColor Cyan
    Invoke-SignFile (Join-Path $Release 'JiYuTrainer.exe')
    $sig = Get-AuthenticodeSignature (Join-Path $Release 'JiYuTrainer.exe')
    Write-Host "Signature subject: $($sig.SignerCertificate.Subject)"
    Write-Host "Signature status : $($sig.Status) (self-signed, not trusted by default)"
}

# ---------------------------------------------------------------- tests
if (-not $SkipTests) {
    Write-Host '== Build core tests ==' -ForegroundColor Cyan
    $TestOut = Join-Path $Build 'tests'
    New-Item -ItemType Directory -Force $TestOut | Out-Null
    $env:INCLUDE = "$($Msvc.FullName)\include;$SdkRoot\Include\$SdkVer\ucrt;$SdkRoot\Include\$SdkVer\shared;$SdkRoot\Include\$SdkVer\um;$SdkRoot\Include\$SdkVer\winrt;$SdkRoot\Include\$SdkVer\cppwinrt"
    $env:LIB = "$($Msvc.FullName)\lib\x86;$SdkRoot\Lib\$SdkVer\ucrt\x86;$SdkRoot\Lib\$SdkVer\um\x86"
    $env:PATH = "$($Msvc.FullName)\bin\Hostx64\x86;$($Msvc.FullName)\bin\Hostx64\x64;$env:PATH"
    & $ClExe /nologo /std:c++17 /EHsc /W3 /D_CRT_SECURE_NO_WARNINGS `
        /I (Join-Path $Src 'JiYuTrainer') `
        "/Fe:$TestOut\CoreTests.exe" `
        (Join-Path $Src 'tests\CoreTests.cpp') `
        (Join-Path $Src 'JiYuTrainer\IpUtils.cpp') `
        (Join-Path $Src 'JiYuTrainer\JyPacket.cpp')
    if ($LASTEXITCODE -ne 0) { throw 'CoreTests compilation failed' }
}

# ---------------------------------------------------------------- package
Write-Host '== Package dist ==' -ForegroundColor Cyan
if (Test-Path $Dist) { Remove-Item $Dist -Recurse -Force }
New-Item -ItemType Directory -Force $Dist | Out-Null
Copy-Item (Join-Path $Release 'JiYuTrainer.exe') $Dist -Force
Copy-Item (Join-Path $Release 'JiYuTrainerHooks.dll') $Dist -Force
Copy-Item $SciterDll $Dist -Force
Copy-Item (Join-Path $Src 'LICENSE') $Dist -Force
Copy-Item (Join-Path $RepoRoot 'README-REFACTOR.md') $Dist -Force
Copy-Item (Join-Path $RepoRoot 'NOTICE') $Dist -Force
$zip = Join-Path $RepoRoot 'JiYuTrainer-JareLi-1.7.8.zip'
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path (Join-Path $Dist '*') -DestinationPath $zip -Force

Write-Host ''
Write-Host 'Build complete.' -ForegroundColor Green
Write-Host "  exe : $(Join-Path $Release 'JiYuTrainer.exe')"
Write-Host "  dist: $Dist"
Write-Host "  zip : $zip"
if (-not $SkipTests) {
    Write-Host ''
    Write-Host 'Next: python tests\run_tests.py' -ForegroundColor Yellow
}
