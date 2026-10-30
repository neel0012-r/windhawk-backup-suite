$csharp = @"
using System;
using System.Runtime.InteropServices;

public class Win32Res {
    [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Unicode)]
    public static extern IntPtr BeginUpdateResource(string pFileName, bool bDeleteExistingResources);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool UpdateResource(IntPtr hUpdate, IntPtr lpType, IntPtr lpName, ushort wLanguage, byte[] lpData, uint cbData);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool EndUpdateResource(IntPtr hUpdate, bool fDiscard);
}
"@

Add-Type -TypeDefinition $csharp -ErrorAction SilentlyContinue

function Embed-Icon {
    param(
        [string]$exePath,
        [string]$icoPath
    )

    $icoBytes = [System.IO.File]::ReadAllBytes($icoPath)
    $ms = New-Object System.IO.MemoryStream(,$icoBytes)
    $br = New-Object System.IO.BinaryReader($ms)

    $idReserved = $br.ReadUInt16()
    $idType = $br.ReadUInt16()
    $count = $br.ReadUInt16()

    Write-Host "Embedding $count icon resolutions into $exePath..."

    $hUpdate = [Win32Res]::BeginUpdateResource($exePath, $false)
    if ($hUpdate -eq [IntPtr]::Zero) {
        Write-Error "Failed to open $exePath for resource update."
        return $false
    }

    # Group Icon Header
    $grpMs = New-Object System.IO.MemoryStream
    $grpBw = New-Object System.IO.BinaryWriter($grpMs)
    $grpBw.Write([UInt16]0)
    $grpBw.Write([UInt16]1)
    $grpBw.Write([UInt16]$count)

    $entries = @()
    for ($i = 0; $i -lt $count; $i++) {
        $entry = @{
            Width = $br.ReadByte()
            Height = $br.ReadByte()
            ColorCount = $br.ReadByte()
            Reserved = $br.ReadByte()
            Planes = $br.ReadUInt16()
            BitCount = $br.ReadUInt16()
            BytesInRes = $br.ReadUInt32()
            ImageOffset = $br.ReadUInt32()
            Id = $i + 1
        }
        $entries += $entry

        $grpBw.Write([byte]$entry.Width)
        $grpBw.Write([byte]$entry.Height)
        $grpBw.Write([byte]$entry.ColorCount)
        $grpBw.Write([byte]$entry.Reserved)
        $grpBw.Write([UInt16]$entry.Planes)
        $grpBw.Write([UInt16]$entry.BitCount)
        $grpBw.Write([UInt32]$entry.BytesInRes)
        $grpBw.Write([UInt16]$entry.Id)
    }

    $RT_ICON = [IntPtr]3
    $RT_GROUP_ICON = [IntPtr]14
    $LANG_NEUTRAL = [UInt16]0

    foreach ($e in $entries) {
        $imgData = New-Object byte[] $e.BytesInRes
        [Array]::Copy($icoBytes, $e.ImageOffset, $imgData, 0, $e.BytesInRes)
        $resName = [IntPtr]$e.Id
        [void][Win32Res]::UpdateResource($hUpdate, $RT_ICON, $resName, $LANG_NEUTRAL, $imgData, [uint32]$imgData.Length)
    }

    $grpData = $grpMs.ToArray()
    $grpName = [IntPtr]1
    [void][Win32Res]::UpdateResource($hUpdate, $RT_GROUP_ICON, $grpName, $LANG_NEUTRAL, $grpData, [uint32]$grpData.Length)

    $grpBw.Dispose()
    $grpMs.Dispose()
    $br.Dispose()
    $ms.Dispose()

    $result = [Win32Res]::EndUpdateResource($hUpdate, $false)
    Write-Host "Icon embedding completed: $result"
    return $result
}

$exe = "C:\Users\Admin\.gemini\antigravity\scratch\WindhawkVault\WindhawkBackupSuite.exe"
$ico = "C:\Users\Admin\.gemini\antigravity\scratch\WindhawkVault\assets\app_icon.ico"
Embed-Icon $exe $ico
