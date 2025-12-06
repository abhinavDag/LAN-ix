#!/bin/bash

dump_dir() {
    local dir="$1"

    for f in "$dir"/*; do
        [ -e "$f" ] || continue

        if [ -d "$f" ]; then
            dump_dir "$f"        # recurse into directories
        elif [ -f "$f" ]; then
            echo
            echo "=============================="
            echo "FILE: $f"
            echo "=============================="
            echo
            cat "$f"
            echo
            echo
        fi
    done
}

dump_dir .

