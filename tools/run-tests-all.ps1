$PSDefaultParameterValues['Out-File:Encoding'] = 'ASCII'
$utf8 = New-Object System.Text.UTF8Encoding $false

function printContents([string[]] $contents)
{
  foreach ($line in $contents)
  {
    Write-Output "-- $line"
  }  
}

$scriptPath = split-path -parent $MyInvocation.MyCommand.Definition

$compilers  = "vs2015","vs2017","vs2019"

#TODO: consider building [Debug|Release]
#$configs    = "Debug","Release"
$configs    = "nostd","stl"

#TODO: consider cross-compiling for targets below
#$platforms  = "Win32","x64","ARM64"

cd $scriptPath
$outdir = "$scriptPath\..\out"
if ($args.count -eq 1)
{
  $outdir = $args[0]
}

foreach($compiler in $compilers)
{
  foreach($config in $configs)
  {
    $testdir = "$outdir\$compiler\$config"
    if ( Test-Path -Path $testdir -PathType Container )
    {
      Write-Output "Testing $testdir"
      ./run-tests.cmd $testdir | Tee-Object -Variable out
      # Parse combined Google Test output from all tests run

      #      
      # passed.json
      #
      $allText = "["
      $testsOK = $out | Select-String -Pattern '\[[ ]+OK \] (.+) \((\d+) ms\)'
      $isFirst = 1
      foreach ($match in $testsOK.Matches)
      {
        if ( $isFirst -eq 0 ) {
          $allText += ","
        }
        $allText+="{ `"name`": `""+$match.Groups[1].Value+"`", `"duration`": "+$match.Groups[2].Value+" }"
        $isFirst = 0
      }
      $allText += "]"
      Set-Content -Value $utf8.GetBytes($allText) -Encoding Byte -Path $testdir\passed.json

      #      
      # failed.json
      #
      # $testsFailedCount = $out | Select-String -Pattern '\[[ ]+FAILED[ ]+\] (\d+) test[.]*'
      $testsFailed = $out | Select-String -Pattern '\[[ ]+FAILED[ ]+\] ([A-Za-z]+[A-Za-z\S]*)[.]*'
      $allText = "["
      $isFirst = 1
      foreach ($match in $testsFailed.Matches)
      {
        Write-Host $match.Groups[1].Value
        if ( $isFirst -eq 0 ) {
          $allText += ","
        }
        $allText+="{ `"name`": `""+$match.Groups[1].Value+"`" }"
        $isFirst = 0
      }
      $allText += "]"
      Set-Content -Value $utf8.GetBytes($allText) -Encoding Byte -Path $testdir\failed.json

    }
  }
}
