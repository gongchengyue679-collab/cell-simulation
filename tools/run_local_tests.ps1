param([string]$CompilerPath='C:/msys64/ucrt64/bin/g++.exe')
$ErrorActionPreference = 'Stop'
if (-not (Test-Path -LiteralPath $CompilerPath)) { throw '请指定已有 C++ 编译器的完整路径。' }
$root = Split-Path -Parent $PSScriptRoot
$destination = Join-Path $root 'output/verification/native-v8-tests.exe'
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $destination) | Out-Null
$compileTemp = Join-Path $root '.cache/build'
New-Item -ItemType Directory -Force -Path $compileTemp | Out-Null
$previousTemp = $env:TEMP
$previousTmp = $env:TMP
try {
    $env:TEMP = $compileTemp
    $env:TMP = $compileTemp
    Push-Location -LiteralPath (Split-Path -Parent $CompilerPath)
    try {
        & $CompilerPath '-std=c++20' '-O2' '-static' (Join-Path $PSScriptRoot 'native_cooperation_test.cpp') '-o' $destination
        if ($LASTEXITCODE -ne 0) { throw '局部机制测试编译失败。' }
    } finally { Pop-Location }
} finally { $env:TEMP = $previousTemp; $env:TMP = $previousTmp }
& $destination
if ($LASTEXITCODE -ne 0) { throw '局部机制测试未通过。' }
