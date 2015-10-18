#bin/sh
flags="-lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -lXxf86vm -lm -ldl -lXinerama -lXcursor -lrt -std=c++0x"
executable_dir="executables"
compiler="g++"
file_ending=".c"

if [ ! -d "$executable_dir" ]; then
  mkdir "$executable_dir"
fi

if [ ! -z "$1" ]; then
  case "$1" in
    "--clean")
      echo "Cleaning out executables and recompiling.."
      rm -rf "$executable_dir"
      mkdir "$executable_dir"
    ;;
    *)
      echo `printf "No such command: %s" $1`
      exit 1
    ;;
  esac
fi

for file in ./*; do
  case "$file" in
    *"$file_ending"*)
      exec_name=$(echo $file | cut -c 2- | rev | cut -c 3- | rev)
      $compiler "$file" -o "${executable_dir}/${exec_name}.exe" $flags
      ;;
  esac
done
