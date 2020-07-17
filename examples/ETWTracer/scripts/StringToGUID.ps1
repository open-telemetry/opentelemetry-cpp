param([string]$token)
$source = Get-Content -Path "StringToGUID.cs"
Add-Type -TypeDefinition "$source"
$obj = New-Object MakeGuid
$obj.GetProviderGuid($token).ToString("B")

