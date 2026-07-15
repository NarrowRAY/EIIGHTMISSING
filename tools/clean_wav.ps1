$music = "D:\PROJECT\Project5\assets\audio\music"
foreach ($name in @("nomai","kazu","timber")) {
    $path = Join-Path $music "$name.wav"
    $bytes = [IO.File]::ReadAllBytes($path)
    $fmtOff = 0
    for ($i=0; $i -lt $bytes.Length-4; $i++) {
        if ($bytes[$i] -eq 102 -and $bytes[$i+1] -eq 109 -and $bytes[$i+2] -eq 116 -and $bytes[$i+3] -eq 32) {
            $fmtOff=$i; break
        }
    }
    $ch = [BitConverter]::ToUInt16($bytes, $fmtOff+10)
    $rate = [BitConverter]::ToUInt32($bytes, $fmtOff+12)
    $bits = [BitConverter]::ToUInt16($bytes, $fmtOff+22)
    $dataOff = 0
    for ($i=$fmtOff+4; $i -lt $bytes.Length-4; $i++) {
        if ($bytes[$i] -eq 100 -and $bytes[$i+1] -eq 97 -and $bytes[$i+2] -eq 116 -and $bytes[$i+3] -eq 97) {
            $dataOff=$i; break
        }
    }
    $dataSize = [BitConverter]::ToUInt32($bytes, $dataOff+4)
    $out = New-Object IO.MemoryStream
    $bw = New-Object IO.BinaryWriter($out)
    $bw.Write([char[]]"RIFF")
    $bw.Write([BitConverter]::GetBytes(36 + $dataSize))
    $bw.Write([char[]]"WAVE")
    $bw.Write([char[]]"fmt ")
    $bw.Write([BitConverter]::GetBytes(16))
    $bw.Write([BitConverter]::GetBytes([UInt16]1))
    $bw.Write([BitConverter]::GetBytes($ch))
    $bw.Write([BitConverter]::GetBytes($rate))
    $bw.Write([BitConverter]::GetBytes($rate * $ch * $bits / 8))
    $bw.Write([BitConverter]::GetBytes($ch * $bits / 8))
    $bw.Write([BitConverter]::GetBytes($bits))
    $bw.Write([char[]]"data")
    $bw.Write([BitConverter]::GetBytes($dataSize))
    $bw.Write($bytes, $dataOff+8, $dataSize)
    [IO.File]::WriteAllBytes($path, $out.ToArray())
    $bw.Close()
    Write-Host "OK: $name.wav ($rate Hz, $bits bit, $ch ch)"
}
