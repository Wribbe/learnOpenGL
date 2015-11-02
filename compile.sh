#bin/sh
flags="-lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -lXxf86vm -lm -ldl -lXinerama -lXcursor -lrt -lasound -lpthread -lportaudio"
executable_dir="executables"
linux_executables="linux"
#compiler="g++"
compiler="cc"
source_folder="sources"
file_ending=".c"

if [ ! -d "$executable_dir" ]; then
  mkdir "$executable_dir"
fi

if [ ! -d "$executable_dir/$linux_executables" ]; then
  mkdir "$executable_dir/$linux_executables"
fi

if [ ! -z "$1" ]; then
  case "$1" in
    "--clean")
      echo "Cleaning out executables and recompiling.."
      rm -rf "$executable_dir"
      mkdir "$executable_dir"
      mkdir "$executable_dir/$linux_executables"
    ;;
    *)
      echo `printf "No such command: %s" $1`
      exit 1
    ;;
  esac
fi

for file in $source_folder; do
  case "$file" in
    *"$file_ending"*)
      exec_name=$(echo $file | cut -c 2- | rev | cut -c 3- | rev)
      $compiler "$file" -o "${executable_dir}/$linux_executables/${exec_name}.exe" $flags
      ;;
  esac
done
