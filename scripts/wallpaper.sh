#!/usr/bin/env bash

set -euo pipefail

DIR="${1:-$HOME/Wallpaper}"

# Build a list safely (handles spaces, many files, and missing extensions)
mapfile -d '' files < <(find "$DIR" -type f \
  \( -iname '*.jpg' -o -iname '*.jpeg' -o -iname '*.png' -o -iname '*.webp' \) -print0)

# Bail if nothing found
(( ${#files[@]} )) || { echo "No images found in $DIR" >&2; exit 1; }

feh "${files[@]}" --bg-fill
