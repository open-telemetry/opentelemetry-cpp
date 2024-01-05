 param ($dumpbin, $bindir, $inputfile, $targetfile)
   
 "LIBRARY opentelemetry_cpp`r`nEXPORTS`r`n" > $targetfile
    
 Get-ChildItem -Verbose -Path $bindir/*.lib -Recurse | % { & "$dumpbin" /SYMBOLS $_ | Select-String -Pattern @(Get-Content -Verbose -Path "$inputfile" | Where-Object { $_.Trim() -ne '' }) | % { "; $($_.matches.groups[2])`r`n$($_.matches.groups[1])" } >> $targetfile }
