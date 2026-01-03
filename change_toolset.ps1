Param(
    [Parameter(Mandatory=$True,Position=1)]
    [string]$toolset,
    [switch]$force
)

# Validate supported toolsets (VS 2019, VS 2022, VS 2026)
$validToolsets = @("v142", "v143", "v145")
if ($toolset -notin $validToolsets) {
    Write-Error "Invalid toolset '$toolset'. Supported toolsets: $($validToolsets -join ', ')"
    Write-Output "  v142 = Visual Studio 2019"
    Write-Output "  v143 = Visual Studio 2022"
    Write-Output "  v145 = Visual Studio 2026"
    exit 1
}

$files = Get-ChildItem -path . -filter *.vcxproj -recurse
foreach ($file in $files) {
    if (!$force.IsPresent -and (($file.basename -eq "vld") -or ($file.basename -eq "format")))
    {
        "Skip $($file.name)"
        continue
    }
    $content = gc $file.fullname
    $regex = [regex]"<PlatformToolset>(v[\d\w_]+)<\/PlatformToolset>"
    $oldtoolset = $regex.matches($content)
    if ($oldtoolset.groups.count -ge 1)
    {
        "  $($file.name) ($($oldtoolset.groups[1].value) - $toolset)"
        $content = $content -replace "<PlatformToolset>(v[\d\w_]+)<\/PlatformToolset>","<PlatformToolset>$toolset</PlatformToolset>"
        $content | sc $file.fullname
    }
    else
    {
        "Toolset not set for $($file.name)"
    }
}

$commonProps = ".\src\tests\Common.props"
if ($force.IsPresent -and (Test-Path $commonProps))
{
    $content = gc $commonProps
    $regex = [regex]"<VldToolset>(v[\d\w_]+)<\/VldToolset>"
    $oldtoolset = $regex.matches($content)
    if ($oldtoolset.groups.count -ge 1)
    {
        "  Common.props ($($oldtoolset.groups[1].value) - $toolset)"
        $content = $content -replace "<VldToolset>(v[\d\w_]+)<\/VldToolset>","<VldToolset>$toolset</VldToolset>"
        $content | sc $commonProps
    }
    else
    {
        "Toolset not set for $commonProps"
    }
}