
#$defaultGUID = "6d084bbf-6a96-44ef-83F4-0a77c9e34580";
$defaultGUID = "b7aa4d18-240c-5f41-5852-817dbf477472";

if ($args.count > 1)
{
  $defaultGUID = $args[ $0 ];
};

while (1)
{
  $pipe=new-object System.IO.Pipes.NamedPipeServerStream("ETW-"+$defaultGUID);
  # Write-Host "Waiting for client connection..."
  $pipe.WaitForConnection();
  $sr = new-object System.IO.StreamReader($pipe);
  try
  {
    # Write-Host "Reading data from client..."
    while ($cmd= $sr.ReadLine()) 
    {
      $cmd
    };
  } catch [System.IO.IOException]
  {
    # Write-Host "Pipe closed."    
  }
  $sr.Dispose();
  $pipe.Dispose();
}
