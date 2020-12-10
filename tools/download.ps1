$url=$args[0]
$arr=$args[0].Split("/")
$fileName=$arr[$arr.Count-1]
Invoke-WebRequest -Uri $url -OutFile $fileName -UseBasicParsing
