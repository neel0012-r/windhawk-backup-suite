Add-Type -AssemblyName System.Drawing

function Create-AppIcon {
    param([string]$outputPath)

    $sizes = @(256, 128, 64, 48, 32, 16)
    $bitmaps = @()

    foreach ($s in $sizes) {
        $bmp = New-Object System.Drawing.Bitmap($s, $s, [System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
        $g = [System.Drawing.Graphics]::FromImage($bmp)
        $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
        $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
        $g.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
        $g.Clear([System.Drawing.Color]::Transparent)

        $pad = [float]($s * 0.06)
        $rect = New-Object System.Drawing.RectangleF($pad, $pad, ($s - 2 * $pad), ($s - 2 * $pad))

        # Rounded background container
        $radius = [float]($s * 0.28)
        $path = New-Object System.Drawing.Drawing2D.GraphicsPath
        $d = $radius * 2
        $path.AddArc($rect.X, $rect.Y, $d, $d, 180, 90)
        $path.AddArc($rect.Right - $d, $rect.Y, $d, $d, 270, 90)
        $path.AddArc($rect.Right - $d, $rect.Bottom - $d, $d, $d, 0, 90)
        $path.AddArc($rect.X, $rect.Bottom - $d, $d, $d, 90, 90)
        $path.CloseFigure()

        # Dark base
        $brushBg = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255, 12, 13, 16))
        $g.FillPath($brushBg, $path)

        # Emerald gradient border
        $penBorder = New-Object System.Drawing.Pen([System.Drawing.Color]::FromArgb(220, 0, 240, 118), [float]($s * 0.045))
        $g.DrawPath($penBorder, $path)

        # Inner glyph: Geometric Hawk Wing & Shield
        # Scale coords based on size
        $scale = $s / 100.0

        # Draw 3 sleek sweeping wing feathers
        $colorGreen = [System.Drawing.Color]::FromArgb(255, 0, 240, 118)
        $colorWhite = [System.Drawing.Color]::FromArgb(255, 255, 255, 255)
        $penGreen = New-Object System.Drawing.Pen($colorGreen, [float](4 * $scale))
        $penGreen.StartCap = [System.Drawing.Drawing2D.LineCap]::Round
        $penGreen.EndCap = [System.Drawing.Drawing2D.LineCap]::Round

        $brushGreen = New-Object System.Drawing.SolidBrush($colorGreen)
        $brushWhite = New-Object System.Drawing.SolidBrush($colorWhite)

        # Upper wing feather
        $f1 = New-Object System.Drawing.Drawing2D.GraphicsPath
        $f1.AddBezier(
            [float](28 * $scale), [float](68 * $scale),
            [float](35 * $scale), [float](42 * $scale),
            [float](55 * $scale), [float](26 * $scale),
            [float](76 * $scale), [float](28 * $scale)
        )
        $f1.AddBezier(
            [float](76 * $scale), [float](28 * $scale),
            [float](54 * $scale), [float](36 * $scale),
            [float](42 * $scale), [float](54 * $scale),
            [float](28 * $scale), [float](68 * $scale)
        )
        $g.FillPath($brushWhite, $f1)

        # Mid wing feather (emerald)
        $f2 = New-Object System.Drawing.Drawing2D.GraphicsPath
        $f2.AddBezier(
            [float](30 * $scale), [float](72 * $scale),
            [float](40 * $scale), [float](55 * $scale),
            [float](60 * $scale), [float](46 * $scale),
            [float](74 * $scale), [float](48 * $scale)
        )
        $f2.AddBezier(
            [float](74 * $scale), [float](48 * $scale),
            [float](56 * $scale), [float](54 * $scale),
            [float](44 * $scale), [float](65 * $scale),
            [float](30 * $scale), [float](72 * $scale)
        )
        $g.FillPath($brushGreen, $f2)

        # Central safe vault / shield node
        $nodeRect = New-Object System.Drawing.RectangleF([float](32 * $scale), [float](62 * $scale), [float](16 * $scale), [float](16 * $scale))
        $brushNode = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(255, 0, 240, 118))
        $g.FillEllipse($brushNode, $nodeRect)

        $g.Dispose()
        $bitmaps += $bmp
    }

    # Save to ICO file format
    $fs = [System.IO.File]::Create($outputPath)
    $bw = New-Object System.IO.BinaryWriter($fs)

    # ICONDIR
    $bw.Write([UInt16]0) # idReserved
    $bw.Write([UInt16]1) # idType (1 = icon)
    $bw.Write([UInt16]$bitmaps.Count) # idCount

    # Calculate offsets
    $offset = 6 + ($bitmaps.Count * 16)
    $pngBytesList = @()

    foreach ($b in $bitmaps) {
        $ms = New-Object System.IO.MemoryStream
        $b.Save($ms, [System.Drawing.Imaging.ImageFormat]::Png)
        $bytes = $ms.ToArray()
        $pngBytesList += ,$bytes
        $ms.Dispose()

        # ICONDIRENTRY
        $w = if ($b.Width -ge 256) { 0 } else { $b.Width }
        $h = if ($b.Height -ge 256) { 0 } else { $b.Height }
        $bw.Write([byte]$w)
        $bw.Write([byte]$h)
        $bw.Write([byte]0) # bColorCount
        $bw.Write([byte]0) # bReserved
        $bw.Write([UInt16]1) # wPlanes
        $bw.Write([UInt16]32) # wBitCount
        $bw.Write([UInt32]$bytes.Length) # dwBytesInRes
        $bw.Write([UInt32]$offset) # dwImageOffset
        $offset += $bytes.Length
    }

    foreach ($bytes in $pngBytesList) {
        $bw.Write($bytes)
    }

    $bw.Flush()
    $fs.Dispose()

    foreach ($b in $bitmaps) { $b.Dispose() }
    Write-Host "Icon created successfully at $outputPath"
}

$icoPath = "C:\Users\Admin\.gemini\antigravity\scratch\WindhawkVault\assets\app_icon.ico"
Create-AppIcon $icoPath
