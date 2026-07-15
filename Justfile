CC := "clang"
flags := "-Wall -Wextra"
release_flags := "-O3 -DNDEBUG"
debug_flags := "-g -O0"

[arg("debug", long="debug", value="true")]
[arg("release", long="release", value="true")]
[arg("std")]
@build filepath debug="false" release="false" std="c23": (compile filepath debug release std) (link filepath debug release)
    echo "Done."

[arg("debug", long="debug", value="true")]
[arg("release", long="release", value="true")]
[arg("std")]
compile filepath debug="false" release="false" std="c23":
    #!/bin/sh
    set -eu
    filepath="{{filepath}}"
    filepath="${filepath%/.}"

    if [ ! -e "$filepath" ]; then
        echo "Error: $filepath: No such file or directory" >&2
        exit 1
    fi
    
    mode="{{ if release == "true" { "release" } else { "debug" } }}"
    files="$(find "$filepath" -depth -type f -name "*.c" | sort)"
    for file in $files; do
        dirname="$(dirname $file)"
        filename="$(basename $file)"
        build_dir="./build/$mode/objects/$dirname"
        object_file="${build_dir%/}/${filename%.c}.o"
        mkdir -p "$build_dir"
        if [ ! -e "$object_file" ] || { [ -f "$object_file" ] && [ "$file" -nt "$object_file" ]; }; then
            echo "Building \"$file\""

            include_def=""
            if [ -f "$filepath/include.def" ]; then
                include_def="$(cat "$filepath/include.def")"
            fi
            
            if [ -n "$(printf '%s' "$include_def" | tr -d '[:space:]')" ]; then
                printf '%s' "$include_def" | xargs {{CC}} {{flags}} {{ if release == "true" { release_flags } else { debug_flags } }} -c "$file" -o "$object_file"
            else
                {{CC}} {{flags}} {{ if release == "true" { release_flags } else { debug_flags } }} -std={{std}} -c "$file" -o "$object_file"
            fi
        fi
    done

[arg("debug", long="debug", value="true")]
[arg("release", long="release", value="true")]
link filepath debug="false" release="false":
    #!/bin/sh
    set -eu
    filepath="{{filepath}}"
    filepath="${filepath%/.}"

    if [ ! -e "$filepath" ]; then
        echo "Error: $filepath: No such file or directory" >&2
        exit 1
    fi

    mode="{{ if release == "true" { "release" } else { "debug" } }}"
    files="$(find "./build/$mode/objects/$filepath" -depth -type f -name "*.o" | sort)"
    binary="$(basename $filepath)"
    if [ "$binary" = "." ] || [ "$binary" = ".." ]; then
        binary="bin"
    fi
    mkdir -p "./build/$mode/results/$filepath"
    echo "Linking to \"./build/$mode/results/$filepath/$binary\""

    link_def=""
    if [ -f "$filepath/link.def" ]; then
        link_def="$(cat "$filepath/link.def")"
    fi
            
    if [ -n "$(printf '%s' "$link_def" | tr -d '[:space:]')" ]; then
        printf '%s' "$link_def" | xargs {{CC}} $files -o "./build/$mode/results/$filepath/$binary"
    else
        {{CC}} $files -o "./build/$mode/results/$filepath/$binary"
    fi

[confirm("Clean the build? [y/n] ")]
@clean:
    echo "Cleaning the build.."
    rm -rv build
    echo "Done."

run filepath: (build filepath "true" "false")
    #!/bin/sh
    set -eu
    filepath="{{filepath}}"
    filepath="${filepath%/.}"
    binary="$(basename $filepath)"
    echo
    "./build/debug/results/$filepath/$binary"
