param([ValidateSet('V6','V7','V8')][string]$Version='V8', [string]$CompilerPath='')
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$outName = 'release-' + $Version.ToLower()
if ($Version -eq 'V6') { $outName = 'release-cpp' }
$exeName = '细胞模拟' + $Version + '.exe'
$out = Join-Path $root "output\$outName"
New-Item -ItemType Directory -Force -Path $out | Out-Null
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
if ($CompilerPath -or -not (Test-Path -LiteralPath $vswhere)) {
    if (-not $CompilerPath) { $CompilerPath = 'C:\msys64\ucrt64\bin\g++.exe' }
    if (-not (Test-Path -LiteralPath $CompilerPath)) { throw '没有找到现有 C++ 编译器；请通过 -CompilerPath 指定绝对路径。' }
    $flags = @('-std=c++20', '-O2', '-static', '-mwindows', '-municode', '-DUNICODE', '-D_UNICODE', '-DNOMINMAX')
    if ($Version -ne 'V6') { $flags += '-DCELL_SIM_' + $Version }
    $taskTemp = $env:TEMP; $taskTmp = $env:TMP
    $buildTemp = Join-Path $root '.cache\build'
    New-Item -ItemType Directory -Force -Path $buildTemp | Out-Null
    try {
        $env:TEMP = $buildTemp; $env:TMP = $buildTemp
        Push-Location -LiteralPath (Split-Path -Parent $CompilerPath)
        $compilerOutput = Join-Path $out ('cell-sim-' + $Version.ToLower() + '.exe')
        try { & $CompilerPath @flags (Join-Path $PSScriptRoot 'main.cpp') '-o' $compilerOutput '-lgdiplus' '-lshell32' '-luser32' '-lgdi32' '-lcomdlg32' }
        finally { Pop-Location }
    } finally { $env:TEMP = $taskTemp; $env:TMP = $taskTmp }
    if ($LASTEXITCODE -ne 0) { throw "C++ 编译失败：$LASTEXITCODE" }
    Move-Item -LiteralPath $compilerOutput -Destination (Join-Path $out $exeName) -Force
    Write-Host "已生成：$(Join-Path $out $exeName)"
    return
}
$vs = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vs) { throw '没有找到 Visual Studio C++ x64 工具链。' }
$toolVersion = (Get-Content (Join-Path $vs 'VC\Auxiliary\Build\Microsoft.VCToolsVersion.default.txt')).Trim()
$msvc = Join-Path $vs "VC\Tools\MSVC\$toolVersion"
$sdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits\10'
$sdkVersion = (Get-ChildItem (Join-Path $sdkRoot 'Lib') -Directory | Sort-Object Name -Descending | Select-Object -First 1).Name
$outName = if ($Version -eq 'V8') { 'release-v8' } elseif ($Version -eq 'V7') { 'release-v7' } else { 'release-cpp' }
$exeName = if ($Version -eq 'V8') { '细胞模拟V8.exe' } elseif ($Version -eq 'V7') { '细胞模拟V7.exe' } else { '细胞模拟V6.exe' }
$out = Join-Path $root "output\$outName"
New-Item -ItemType Directory -Force -Path $out | Out-Null
$compiler = Join-Path $msvc 'bin\Hostx64\x64\cl.exe'
$arguments = @(
    '/nologo', '/std:c++20', '/O2', '/GL', '/EHsc', '/utf-8', '/MT',
    '/DUNICODE', '/D_UNICODE', '/DNOMINMAX',
    '/I', $PSScriptRoot,
    '/I', (Join-Path $msvc 'include'),
    '/I', (Join-Path $sdkRoot "Include\$sdkVersion\shared"),
    '/I', (Join-Path $sdkRoot "Include\$sdkVersion\um"),
    '/I', (Join-Path $sdkRoot "Include\$sdkVersion\ucrt"),
    ('/Fo' + $out + '\'),
    ('/Fe' + $out + '\' + $exeName),
    (Join-Path $PSScriptRoot 'main.cpp'),
    '/link', '/LTCG', '/OPT:REF', '/OPT:ICF', '/SUBSYSTEM:WINDOWS', '/MACHINE:X64',
    ('/LIBPATH:' + (Join-Path $msvc 'lib\x64')),
    ('/LIBPATH:' + (Join-Path $sdkRoot "Lib\$sdkVersion\um\x64")),
    ('/LIBPATH:' + (Join-Path $sdkRoot "Lib\$sdkVersion\ucrt\x64")),
    (Join-Path $sdkRoot "Lib\$sdkVersion\um\x64\gdiplus.lib"),
    (Join-Path $sdkRoot "Lib\$sdkVersion\um\x64\shell32.lib"),
    (Join-Path $sdkRoot "Lib\$sdkVersion\um\x64\user32.lib"),
    (Join-Path $sdkRoot "Lib\$sdkVersion\um\x64\gdi32.lib"),
    (Join-Path $sdkRoot "Lib\$sdkVersion\um\x64\comdlg32.lib")
)
if ($Version -eq 'V8') { $arguments = @($arguments[0..9] + @('/DCELL_SIM_V8') + $arguments[10..($arguments.Count - 1)]) }
if ($Version -eq 'V7') { $arguments = @($arguments[0..9] + @('/DCELL_SIM_V7') + $arguments[10..($arguments.Count - 1)]) }
& $compiler @arguments
if ($LASTEXITCODE -ne 0) { throw "C++ 编译失败：$LASTEXITCODE" }
Write-Host "已生成：$(Join-Path $out $exeName)"
