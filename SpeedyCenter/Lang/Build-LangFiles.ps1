#requires -version 5.1
<#
.SYNOPSIS
    Generates the SpeedyCenter language files (SpeedyCenter.CHS / .CHT / .ENU) from the
    translations in xliff\<Lang>\. The files are read at runtime by LangRes.pas.

.DESCRIPTION
    A language file holds only the translated DIFFERENCES:
      * form properties, keyed by form class + component path (from xliff\<Lang>\*.dfn)
      * resourcestrings, keyed by their original text      (from xliff\<Lang>\SpeedyCenter_DRC.rcn)
    Nothing in it depends on SpeedyCenter.exe (no resource IDs, no copies of forms), so
    changing a .dfm or SCStrings.pas only needs a build of SpeedyCenter. At runtime a
    translation is used only while the form / string still holds the original it was
    translated from; otherwise the current original is shown.

    Normal run (post-build event of SpeedyCenter.cbproj): xliff -> language files. Skipped
    when the language files are newer than all XLIFF files and this script.

.PARAMETER OutDir
    Where to write SpeedyCenter.<Lang> (next to SpeedyCenter.exe). Default: ..\Debug

.PARAMETER Lang
    Languages to generate. Default: CHS, CHT, ENU.

.PARAMETER Check
    Also compare the translations with the current forms and with the resourcestrings of
    the last build (<OutDir>\SpeedyCenter.drc) and list in gen\<Lang>\report.txt what
    is out of date (stale: original changed after it was translated; gone: component or
    property removed; new: text without an XLIFF entry).

.PARAMETER UpdateXliff
    Implies -Check. Adds <trans-unit>s (source only, no target) for texts that have no
    entry yet - new forms, new components, new resourcestrings - so translators can fill
    in the <target>.

.PARAMETER Force
    Regenerate even when the language files are up to date.

.NOTES
    <OutDir>\SpeedyCenter.drc (written by the linker) is used, when present, by -Check /
    -UpdateXliff and to tell apart resourcestrings that share one original but have
    different translations (those few are also written with the resource ID of that build).
#>
[CmdletBinding()]
param(
    [string]$OutDir,
    [ValidateSet('CHS', 'CHT', 'ENU')][string[]]$Lang = @('CHS', 'CHT', 'ENU'),
    [switch]$Check,
    [switch]$UpdateXliff,
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version 2

$LangDir = $PSScriptRoot
$Root    = Split-Path $LangDir -Parent
$Cbproj  = Join-Path $Root 'SpeedyCenter.cbproj'
if (-not $OutDir) { $OutDir = Join-Path $Root 'Debug' }
$OutDir  = [System.IO.Path]::GetFullPath($OutDir)
$Drc     = Join-Path $OutDir 'SpeedyCenter.drc'      # written by the linker (ILINK_GenerateDRC)
$Utf8    = New-Object System.Text.UTF8Encoding($false)
if ($UpdateXliff) { $Check = $true }

function Fail([string]$msg) { Write-Host "Build-LangFiles: error: $msg"; exit 1 }

# --------------------------------------------------------------------------- DFM text helpers

# Last significant character of a DFM line, ignoring anything inside '...' literals.
function Get-LastCodeChar([string]$s) {
    $in = $false; $last = $null
    foreach ($c in $s.ToCharArray()) {
        if ($c -eq "'") { $in = -not $in; $last = "'"; continue }
        if (-not $in -and -not [char]::IsWhiteSpace($c)) { $last = [string]$c }
    }
    return $last
}

# 'abc'#13#10'def' + 'g'  ->  string ; $null if the expression is not a string.
function ConvertFrom-DfmString([string]$expr) {
    $sb = New-Object System.Text.StringBuilder
    $i = 0; $n = $expr.Length; $any = $false
    while ($i -lt $n) {
        $c = $expr[$i]
        if ($c -eq "'") {
            $i++
            while ($true) {
                if ($i -ge $n) { return $null }
                $d = $expr[$i]
                if ($d -eq "'") {
                    if ($i + 1 -lt $n -and $expr[$i + 1] -eq "'") { [void]$sb.Append("'"); $i += 2; continue }
                    $i++; break
                }
                [void]$sb.Append($d); $i++
            }
            $any = $true; continue
        }
        if ($c -eq '#') {
            $j = $i + 1
            while ($j -lt $n -and [char]::IsDigit($expr[$j])) { $j++ }
            if ($j -eq $i + 1) { return $null }
            [void]$sb.Append([char][int]$expr.Substring($i + 1, $j - $i - 1)); $i = $j; $any = $true; continue
        }
        if ($c -eq '+' -or [char]::IsWhiteSpace($c)) { $i++; continue }
        return $null
    }
    if ($any) { return $sb.ToString() }
    return $null
}

# string -> ASCII-only DFM literal ('Login'#30331#20837)
function ConvertTo-DfmString([string]$s) {
    if ($s.Length -eq 0) { return "''" }
    $sb = New-Object System.Text.StringBuilder; $inq = $false
    foreach ($ch in $s.ToCharArray()) {
        $code = [int]$ch
        if ($code -ge 0x20 -and $code -le 0x7E) {
            if (-not $inq) { [void]$sb.Append("'"); $inq = $true }
            if ($ch -eq "'") { [void]$sb.Append("''") } else { [void]$sb.Append($ch) }
        } else {
            if ($inq) { [void]$sb.Append("'"); $inq = $false }
            [void]$sb.Append('#').Append($code)
        }
    }
    if ($inq) { [void]$sb.Append("'") }
    return $sb.ToString()
}

function Test-SameValue([string]$a, [string]$b) {
    $da = ConvertFrom-DfmString $a; $db = ConvertFrom-DfmString $b
    if ($null -ne $da -and $null -ne $db) { return $da -ceq $db }
    return $a.Trim() -ceq $b.Trim()
}

function Get-Indent([string]$line) { return $line.Substring(0, $line.Length - $line.TrimStart().Length) }

# Parses a text DFM into property records keyed by ITE-style paths, e.g.
#   O:SimTFXForm.O:ProcessListView.Columns[0].Caption
#   O:SimTFXForm.O:AppComboBox.Items.Strings[1]
function Read-Dfm([string[]]$L) {
    $props = New-Object 'System.Collections.Generic.Dictionary[string,object]'
    $stack = New-Object System.Collections.ArrayList
    $i = 0
    while ($i -lt $L.Count) {
        $raw = $L[$i]; $t = $raw.Trim()
        if ($t -match '^(object|inherited|inline)\s+(\w+)\s*:') {
            [void]$stack.Add(@{ Kind = 'O'; Name = $Matches[2] }); $i++; continue
        }
        if ($t -match '^(object|inherited|inline)\s+\w+') {        # unnamed component, e.g. "object TMenuItem"
            [void]$stack.Add(@{ Kind = 'O'; Name = "#$i" }); $i++; continue
        }
        if ($t -eq 'end') { $stack.RemoveAt($stack.Count - 1); $i++; continue }
        if ($t -match '^end\s*>$') { $stack.RemoveAt($stack.Count - 1); $stack.RemoveAt($stack.Count - 1); $i++; continue }
        if ($t -match '^item(\s|$)') {
            $coll = $stack[$stack.Count - 1]
            [void]$stack.Add(@{ Kind = 'I'; Name = "[$($coll.N)]" }); $coll.N++; $i++; continue
        }
        if ($t -match '^([\w\.]+)\s*=\s*(.*)$') {
            $key = $Matches[1]; $val = $Matches[2]
            $prefix = ''
            foreach ($f in $stack) {
                switch ($f.Kind) { 'O' { $prefix += "O:$($f.Name)." } 'C' { $prefix += $f.Name } 'I' { $prefix += "$($f.Name)." } }
            }
            if ($val -eq '<') { [void]$stack.Add(@{ Kind = 'C'; Name = $key; N = 0 }); $i++; continue }
            if ($val -eq '<>') { $i++; continue }
            if ($val.StartsWith('{')) {
                while ((Get-LastCodeChar $L[$i]) -ne '}') { $i++ }
                $i++; continue
            }
            if ($val.StartsWith('(')) {
                $rest = $val.Substring(1).Trim()
                if ($rest -ne '') {                       # "()" or items on the same line: not translatable
                    while ((Get-LastCodeChar $L[$i]) -ne ')') { $i++ }
                    $i++; continue
                }
                $j = $i + 1; $idx = 0
                while ($true) {
                    $s = $j
                    while ((Get-LastCodeChar $L[$j]) -eq '+') { $j++ }
                    $isLast = (Get-LastCodeChar $L[$j]) -eq ')'
                    $text = (($L[$s..$j] | ForEach-Object { $_.Trim() }) -join ' ')
                    if ($isLast) { $text = $text.Substring(0, $text.LastIndexOf(')')).TrimEnd() }
                    $props[('{0}{1}[{2}]' -f $prefix, $key, $idx)] = [pscustomobject]@{
                        Start = $s; End = $j; Indent = (Get-Indent $L[$s]); Key = $null; Last = $isLast; Text = $text }
                    $idx++; $j++
                    if ($isLast) { break }
                }
                $i = $j; continue
            }
            $j = $i
            while ((Get-LastCodeChar $L[$j]) -eq '+') { $j++ }
            $text = $val
            if ($j -gt $i) { $text += ' ' + (($L[($i + 1)..$j] | ForEach-Object { $_.Trim() }) -join ' ') }
            $props["$prefix$key"] = [pscustomobject]@{
                Start = $i; End = $j; Indent = (Get-Indent $raw); Key = $key; Last = $false; Text = $text }
            $i = $j + 1; continue
        }
        $i++
    }
    return $props
}

# Properties worth offering to translators when -UpdateXliff adds new entries.
function Test-TextProperty([string]$path) {
    return $path -match '\.(Caption|Hint|Text|Filter|Title|DisplayLabel|TextHint)$' -or
           $path -match '\.(Items|Lines|Strings)\.Strings\[\d+\]$' -or $path -match '\.Strings\[\d+\]$'
}

# --------------------------------------------------------------------------- XLIFF helpers

function Read-Xliff([string]$path) {
    $settings = New-Object System.Xml.XmlReaderSettings
    $settings.DtdProcessing = [System.Xml.DtdProcessing]::Parse
    $settings.XmlResolver = $null
    $doc = New-Object System.Xml.XmlDocument
    $doc.PreserveWhitespace = $true
    $reader = [System.Xml.XmlReader]::Create($path, $settings)
    try { $doc.Load($reader) } finally { $reader.Close() }
    return $doc
}

function Get-XliffUnits($doc) {
    $units = New-Object 'System.Collections.Generic.Dictionary[string,object]'
    foreach ($tu in $doc.SelectNodes('//body/trans-unit')) {
        $src = $tu.SelectSingleNode('source'); $tgt = $tu.SelectSingleNode('target')
        $units[$tu.GetAttribute('id')] = [pscustomobject]@{
            Source = $(if ($src) { $src.InnerText } else { '' })
            Target = $(if ($tgt) { $tgt.InnerText } else { $null }) }
    }
    return $units
}

function New-XliffDoc([string]$original, [string]$datatype, [string]$lang) {
    $tl = @{ CHS = 'zh-CN'; CHT = 'zh-TW'; ENU = 'en-US' }[$lang]
    $xml = @"
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE xliff PUBLIC "-//XLIFF//DTD XLIFF//EN" "http://www.oasis-open.org/committees/xliff/documents/xliff.dtd">
<xliff version="1.0">
  <file original="$original" source-language="zh-TW" target-language="$tl" datatype="$datatype" tool="Build-LangFiles.ps1">
    <body>
    </body>
  </file>
</xliff>
"@
    $settings = New-Object System.Xml.XmlReaderSettings
    $settings.DtdProcessing = [System.Xml.DtdProcessing]::Parse
    $settings.XmlResolver = $null
    $doc = New-Object System.Xml.XmlDocument
    $doc.PreserveWhitespace = $true
    $doc.Load([System.Xml.XmlReader]::Create((New-Object System.IO.StringReader($xml)), $settings))
    return $doc
}

function Add-XliffUnit($doc, [string]$id, [string]$source) {
    $body = $doc.SelectSingleNode('//body')
    $tu = $doc.CreateElement('trans-unit')
    $tu.SetAttribute('id', $id); $tu.SetAttribute('resname', $id)
    $src = $doc.CreateElement('source'); $src.InnerText = $source
    [void]$tu.AppendChild($doc.CreateTextNode("`r`n        "))
    [void]$tu.AppendChild($src)
    [void]$tu.AppendChild($doc.CreateTextNode("`r`n      "))
    $last = $body.LastChild
    if ($last -and $last.NodeType -eq [System.Xml.XmlNodeType]::Whitespace) {
        [void]$body.InsertBefore($doc.CreateTextNode("`r`n      "), $last)
        [void]$body.InsertBefore($tu, $last)
    } else {
        [void]$body.AppendChild($tu)
    }
}

function Save-Xliff($doc, [string]$path) {
    $settings = New-Object System.Xml.XmlWriterSettings
    $settings.Encoding = $Utf8
    $settings.NewLineHandling = [System.Xml.NewLineHandling]::None
    $ms = New-Object System.IO.MemoryStream
    $w = [System.Xml.XmlWriter]::Create($ms, $settings)
    try { $doc.Save($w) } finally { $w.Close() }
    # XmlWriter emits an empty internal subset ("...dtd"[]>); keep the DOCTYPE line as ITE wrote it
    $text = $Utf8.GetString($ms.ToArray()).Replace('.dtd"[]>', '.dtd">')
    [System.IO.File]::WriteAllText($path, $text, $Utf8)
}

# --------------------------------------------------------------------------- language file helpers

# rc / C string literal ("a\"b\n") -> string
function ConvertFrom-CLiteral([string]$lit) {
    $s = $lit.Trim()
    if ($s.Length -lt 2 -or $s[0] -ne '"' -or $s[$s.Length - 1] -ne '"') { return $null }
    $s = $s.Substring(1, $s.Length - 2)
    $sb = New-Object System.Text.StringBuilder
    $i = 0
    while ($i -lt $s.Length) {
        $c = $s[$i]
        if ($c -ne '\' -or $i + 1 -ge $s.Length) { [void]$sb.Append($c); $i++; continue }
        $d = $s[$i + 1]; $i += 2
        switch -CaseSensitive ($d) {
            'n' { [void]$sb.Append("`n") }
            'r' { [void]$sb.Append("`r") }
            't' { [void]$sb.Append("`t") }
            'a' { [void]$sb.Append([char]7) }
            'b' { [void]$sb.Append([char]8) }
            'f' { [void]$sb.Append([char]12) }
            'v' { [void]$sb.Append([char]11) }
            'x' {
                $j = $i
                while ($j -lt $s.Length -and $j - $i -lt 4 -and [Uri]::IsHexDigit($s[$j])) { $j++ }
                [void]$sb.Append([char][Convert]::ToInt32($s.Substring($i, $j - $i), 16)); $i = $j
            }
            default {
                if ($d -ge '0' -and $d -le '7') {
                    $j = $i - 1
                    while ($j -lt $s.Length -and $j - ($i - 1) -lt 3 -and $s[$j] -ge '0' -and $s[$j] -le '7') { $j++ }
                    [void]$sb.Append([char][Convert]::ToInt32($s.Substring($i - 1, $j - $i + 1), 8)); $i = $j
                } else { [void]$sb.Append($d) }
            }
        }
    }
    return $sb.ToString()
}

function ConvertTo-LangField([string]$s) {
    return $s.Replace('\', '\\').Replace("`t", '\t').Replace("`n", '\n').Replace("`r", '\r')
}

function Get-FormatSpecs([string]$s) {
    return (([regex]::Matches($s, '%(%|-?\d*(\.\d+)?[a-zA-Z])') | ForEach-Object { $_.Value.ToLowerInvariant() }) -join ' ')
}

# DFM value -> @{ Kind = 's'|'i'; Value } ; $null for values that are not translated at runtime
function ConvertFrom-DfmValue([string]$v) {
    $s = ConvertFrom-DfmString $v
    if ($null -ne $s) { return @{ Kind = 's'; Value = $s } }
    $t = $v.Trim()
    if ($t -match '^-?\d+$') { return @{ Kind = 'i'; Value = $t } }
    return $null
}

# --------------------------------------------------------------------------- inputs shared by all languages

[xml]$proj = Get-Content -Raw -Encoding UTF8 $Cbproj
$ns = New-Object System.Xml.XmlNamespaceManager($proj.NameTable)
$ns.AddNamespace('m', 'http://schemas.microsoft.com/developer/msbuild/2003')
$formFiles = @($proj.SelectNodes('//m:FormResources/@Include', $ns) | ForEach-Object { $_.Value })
if ($formFiles.Count -eq 0) { Fail "no <FormResources> found in $Cbproj" }

# form name (= .dfn file name) -> class name, design PixelsPerInch, path of the .dfm
$formInfo = @{}
foreach ($rel in $formFiles) {
    $src = [System.IO.Path]::GetFullPath((Join-Path $Root $rel))
    $base = [System.IO.Path]::GetFileNameWithoutExtension($src)
    $text = [System.IO.File]::ReadAllText($src, [System.Text.Encoding]::Default)
    if ($text -notmatch '^\s*(object|inherited|inline)\s+\w+\s*:\s*(\w+)') { Fail "cannot read the form class from $src" }
    $class = $Matches[2]
    $ppi = 96
    if ($text -match '(?m)^  PixelsPerInch\s*=\s*(\d+)') { $ppi = [int]$Matches[1] }
    $formInfo[$base] = [pscustomobject]@{ Class = $class; Ppi = $ppi; Dfm = $src; Rel = $rel }
}

# incremental: nothing to do when every language file is newer than its inputs
if (-not $Force -and -not $Check) {
    $inputs = @(Get-Item $PSCommandPath, $Cbproj)
    if (Test-Path $Drc) { $inputs += Get-Item $Drc }      # IDs of duplicated originals (see below)
    $upToDate = $true
    foreach ($l in $Lang) {
        $out = Join-Path $OutDir "SpeedyCenter.$l"
        $xd = Join-Path $LangDir "xliff\$l"
        if (-not (Test-Path $out)) { $upToDate = $false; break }
        $newest = @($inputs + @(Get-ChildItem $xd -File -ErrorAction SilentlyContinue)) |
            Sort-Object LastWriteTimeUtc -Descending | Select-Object -First 1
        if ($newest.LastWriteTimeUtc -gt (Get-Item $out).LastWriteTimeUtc) { $upToDate = $false; break }
        $head = [System.IO.File]::ReadAllLines($out, $Utf8) | Select-Object -First 1
        if ($head -ne 'SPEEDYCENTER-LANG 1') { $upToDate = $false; break }   # e.g. an old resource DLL
    }
    if ($upToDate) { Write-Host "Build-LangFiles: language files in $OutDir are up to date"; exit 0 }
}

$drcIds = $null; $drcTable = $null
if ($Check -and -not (Test-Path $Drc)) { Fail "$Drc not found. Build SpeedyCenter first (needed by -Check / -UpdateXliff)." }
if (Test-Path $Drc) {
    $drcIds = @{}; $drcTable = New-Object System.Collections.Generic.List[object]
    foreach ($line in [System.IO.File]::ReadAllLines($Drc, $Utf8)) {
        if ($line -match '^#define\s+(\S+)\s+(\d+)\s*$') { $drcIds[$Matches[1]] = [int]$Matches[2]; continue }
        if ($line -match '^\s*(\w+),\s*(".*")\s*$') { $drcTable.Add([pscustomobject]@{ Name = $Matches[1]; Literal = $Matches[2] }) }
    }
}
$dfmProps = @{}
function Get-DfmProps([string]$base) {
    if (-not $dfmProps.ContainsKey($base)) {
        $lines = [System.IO.File]::ReadAllLines($formInfo[$base].Dfm, [System.Text.Encoding]::Default)
        $dfmProps[$base] = Read-Dfm $lines
    }
    return $dfmProps[$base]
}

New-Item -ItemType Directory -Force $OutDir | Out-Null

foreach ($L in $Lang) {
    $XDir = Join-Path $LangDir "xliff\$L"
    $Gen = Join-Path $LangDir "gen\$L"
    New-Item -ItemType Directory -Force $Gen | Out-Null
    $log = New-Object System.Collections.Generic.List[string]
    $out = New-Object System.Collections.Generic.List[string]
    $out.Add('SPEEDYCENTER-LANG 1')
    $fstat = @{ Entries = 0; Skipped = 0; Stale = 0; Gone = 0; Added = 0 }
    $sstat = @{ Entries = 0; ById = 0; Format = 0; Conflict = 0; Stale = 0; New = 0; Added = 0 }

    # ---------------------------------------------------------------- forms
    $log.Add('== Forms')
    foreach ($base in ($formInfo.Keys | Sort-Object)) {
        $fi = $formInfo[$base]
        $xfile = Join-Path $XDir "$base.dfn"
        $doc = $null; $units = @{}
        if (Test-Path $xfile) { $doc = Read-Xliff $xfile; $units = Get-XliffUnits $doc }
        $props = $null
        if ($Check) { $props = Get-DfmProps $base }

        $lines = New-Object System.Collections.Generic.List[string]
        foreach ($id in $units.Keys) {
            $u = $units[$id]
            if ($null -eq $u.Target -or $u.Target -ceq $u.Source) { continue }
            if ($id -match '\.Name$') { continue }
            $src = ConvertFrom-DfmValue $u.Source; $tgt = ConvertFrom-DfmValue $u.Target
            if ($null -eq $src -or $null -eq $tgt -or $src.Kind -ne $tgt.Kind) {
                $fstat.Skipped++; $log.Add("  [skip ] $base $id : $($u.Source) -> $($u.Target) (only text and integer values are applied)"); continue
            }
            if ($src.Value -ceq $tgt.Value) { continue }
            if ($props) {
                if (-not $props.ContainsKey($id)) { $fstat.Gone++; $log.Add("  [gone ] $base $id") }
                elseif (-not (Test-SameValue $u.Source $props[$id].Text)) {
                    $fstat.Stale++
                    $cur = ConvertFrom-DfmString $props[$id].Text; if ($null -eq $cur) { $cur = $props[$id].Text }
                    $log.Add("  [stale] $base $id"); $log.Add("          translated from: $($src.Value)"); $log.Add("          current        : $cur")
                }
            }
            $lines.Add(('F{0}{1}{0}{2}{0}{3}{0}{4}{0}{5}' -f "`t", $fi.Class, $id, $src.Kind,
                (ConvertTo-LangField $src.Value), (ConvertTo-LangField $tgt.Value)))
        }
        if ($lines.Count -gt 0) {
            $out.Add(('P{0}{1}{0}{2}' -f "`t", $fi.Class, $fi.Ppi))
            $out.AddRange($lines); $fstat.Entries += $lines.Count
        }

        if ($UpdateXliff) {
            $isNew = $false
            if (-not $doc) { $doc = New-XliffDoc $fi.Rel 'delphiform' $L; $isNew = $true }
            $added = 0
            foreach ($id in $props.Keys) {
                if ($units.ContainsKey($id) -or -not (Test-TextProperty $id)) { continue }
                $v = ConvertFrom-DfmString $props[$id].Text
                if ($null -eq $v -or $v -notmatch '[A-Za-z\p{L}]') { continue }
                Add-XliffUnit $doc $id ("'" + $v.Replace("'", "''") + "'"); $added++
            }
            if ($added -gt 0) {
                New-Item -ItemType Directory -Force $XDir | Out-Null
                Save-Xliff $doc $xfile; $fstat.Added += $added
                $log.Add(("  [xliff] {0}: added {1} untranslated entr{2}{3}" -f $base, $added, $(if ($added -eq 1) { 'y' } else { 'ies' }), $(if ($isNew) { ' (new file)' } else { '' })))
            }
        }
    }

    # ---------------------------------------------------------------- resourcestrings
    $log.Add(''); $log.Add('== Strings')
    $rcnFile = Join-Path $XDir 'SpeedyCenter_DRC.rcn'
    $rcnDoc = $null; $rcn = @{}
    if (Test-Path $rcnFile) { $rcnDoc = Read-Xliff $rcnFile; $rcn = Get-XliffUnits $rcnDoc }
    $seen = @{}; $dups = New-Object System.Collections.Generic.List[object]
    foreach ($id in $rcn.Keys) {
        $u = $rcn[$id]
        if ($null -eq $u.Target -or $u.Target -ceq $u.Source) { continue }
        $src = ConvertFrom-CLiteral $u.Source; $tgt = ConvertFrom-CLiteral $u.Target
        if ($null -eq $src -or $null -eq $tgt) { $log.Add("  [skip  ] $id : not a string literal"); continue }
        if ($src -ceq $tgt -or $src.Length -eq 0) { continue }
        if ((Get-FormatSpecs $src) -ne (Get-FormatSpecs $tgt)) {
            $sstat.Format++; $log.Add("  [format] $id : $($u.Target)  vs  $($u.Source)"); continue
        }
        if ($seen.ContainsKey($src)) {
            if ($seen[$src] -cne $tgt) { $dups.Add([pscustomobject]@{ Name = $id; Src = $src; Tgt = $tgt }) }
            continue
        }
        $seen[$src] = $tgt
        $out.Add(('S{0}{1}{0}{2}' -f "`t", (ConvertTo-LangField $src), (ConvertTo-LangField $tgt)))
        $sstat.Entries++
    }
    # The same original with different translations (e.g. "Waiting" as session and as process
    # status) cannot be told apart by text. For those, also write the resource ID of the
    # current build; the runtime uses it only while that ID still loads this very original,
    # so a stale ID falls back to the text entry above.
    foreach ($d in $dups) {
        $rid = $null
        if ($null -ne $drcIds) {
            if ($drcIds.ContainsKey($d.Name)) { $rid = $drcIds[$d.Name] }
            else { foreach ($n in $drcIds.Keys) { if ($n.EndsWith('_' + $d.Name)) { $rid = $drcIds[$n]; break } } }
        }
        if ($null -eq $rid) {
            $sstat.Conflict++
            $log.Add("  [dup   ] $($d.Name) : ""$($d.Src)"" is translated differently elsewhere; no ID in $Drc, the other translation is shown")
            continue
        }
        $out.Add(('I{0}{1}{0}{2}{0}{3}' -f "`t", $rid, (ConvertTo-LangField $d.Src), (ConvertTo-LangField $d.Tgt)))
        $sstat.ById++
    }
    if ($Check) {
        # Older Delphi wrote unit names without namespace (Sysconst_X); now it is System_Sysconst_X.
        foreach ($e in $drcTable) {
            $key = $null
            if ($rcn.ContainsKey($e.Name)) { $key = $e.Name }
            else {
                for ($i = 0; $i -lt $e.Name.Length; $i++) {
                    if ($e.Name[$i] -eq '_') { $s = $e.Name.Substring($i + 1); if ($rcn.ContainsKey($s)) { $key = $s; break } }
                }
            }
            if ($null -eq $key) {
                $sstat.New++
                if ($UpdateXliff) {
                    if (-not $rcnDoc) { $rcnDoc = New-XliffDoc 'SpeedyCenter.drc' 'cpp' $L }
                    Add-XliffUnit $rcnDoc $e.Name $e.Literal; $sstat.Added++
                }
                continue
            }
            $u = $rcn[$key]
            if ($null -ne $u.Target -and $u.Target -cne $u.Source -and $u.Source -cne $e.Literal) {
                $sstat.Stale++
                $log.Add("  [stale ] $($e.Name)"); $log.Add("           translated from: $($u.Source)"); $log.Add("           current        : $($e.Literal)")
            }
        }
        if ($UpdateXliff -and $sstat.Added -gt 0) {
            New-Item -ItemType Directory -Force $XDir | Out-Null
            Save-Xliff $rcnDoc $rcnFile
            $log.Add("  [xliff] SpeedyCenter_DRC.rcn: added $($sstat.Added) untranslated entries")
        }
    }

    # ---------------------------------------------------------------- output
    $dst = Join-Path $OutDir "SpeedyCenter.$L"
    [System.IO.File]::WriteAllLines($dst, $out, $Utf8)
    $summary = @(
        "SpeedyCenter.${L}: $($fstat.Entries) form values, $($sstat.Entries) strings (+$($sstat.ById) by ID) -> $dst",
        "  skipped: $($fstat.Skipped) form values (not text/integer), $($sstat.Format) strings (format specifiers differ), $($sstat.Conflict) duplicate strings"
    )
    if ($Check) {
        $summary += "  check  : forms $($fstat.Stale) stale, $($fstat.Gone) gone; strings $($sstat.Stale) stale, $($sstat.New) new (no XLIFF entry)"
    }
    if ($UpdateXliff) { $summary += "  xliff  : added $($fstat.Added) form entries and $($sstat.Added) string entries for translators" }
    [System.IO.File]::WriteAllLines((Join-Path $Gen 'report.txt'), ($summary + '' + $log), $Utf8)
    $summary | ForEach-Object { Write-Host $_ }
}
exit 0
