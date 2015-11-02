$src = ".\source"
$file_type = "*.c"
$include_dir = "include_compilation"
$glfw_include = "$include_dir\glfw"

$executables = "executables"
$win_executables = "$executables\win"

$flags = "-lglfw3 -I$glfw_include\include -I$glfw_include\deps -I$glfw_include\src -lgdi32 -lopengl32 -Wl,-subsystem,windows -I. $include_dir\libportaudio.dll.a -lpthread -std=c99 -pedantic -Wall -Wextra -I$include_dir"

$flag_list = $flags.split()

if(!(Test-Path -Path $executables)) {
  New_item -ItemType directory -Path $executables
}

if(!(Test-Path -Path $win_executables)) {
  New-Item -ItemType directory -Path $win_executables
}

Try {
  Get-ChildItem -Path $src | ForEach-Object {
    if ($_.Name -like $file_type) { 
      Write-Output $_.Name
      $stub_name = $_.Name.substring(0,$_.Name.length-2)
      $executable_name = "$stub_name.exe"
      Write-Output $executable_name
      $output_file = "$win_executables\$executable_name" 
      Write-Output $output_file
      mingw32-gcc "$src\$_" -o $output_file $flag_list
    }
  }
}
Catch [System.NotSupportedException] {
  Write-Host "exception.."
}
