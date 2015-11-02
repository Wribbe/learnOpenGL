$src = "."
$file_type = "*.c"

$executables = "executables"
$win_executables = "$executables\win"
$flags = "-static -lglfw3 -Iglfw\include -Iglfw\deps -Iglfw\src -lgdi32 -lopengl32 -Wl,-subsystem,windows -I. libportaudio.dll.a libportaudio-2.dll -lpthread -std=c99 -pedantic -Wall -Wextra"

$flag_list = $flags.split()

if(!(Test-Path -Path $executables)) {
  New_item -ItemType directory -Path $executables
}

if(!(Test-Path -Path $win_executables)) {
  New-Item -ItemType directory -Path $win_executables
}



Get-ChildItem -Path $src | ForEach-Object {
  if ($_.Name -like $file_type) { 
    Write-Output $_.Name
    $stub_name = $_.Name.substring(0,$_.Name.length-2)
    $executable_name = "$stub_name.exe"
    Write-Output $executable_name
    $output_file = "$win_executables\$executable_name" 
    Write-Output $output_file
    mingw32-gcc $_.Name -o $output_file $flag_list
  }
}
