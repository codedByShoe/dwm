#!/usr/bin/env bash

dwmblocks & 
picom &

kdeconnect-indicator &
dunst &

xautolock -time 15 -notify 30 -notifier "notify-send 'Locking in 30 seconds!'" -locker "betterlockscreen -l" &

(
  "$HOME/.local/src/dwm/scripts/wallpaper.sh" "$HOME/Wallpaper"
  while :; do 
    sleep 900
    "$HOME/.local/src/dwm/scripts/wallpaper.sh" "$HOME/Wallpaper"
  done
) &
