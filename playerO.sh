#!/bin/bash

BROKER="34.145.124.95"
P="O"
LOG="/home/nowifibars/playerO.log"

log() { echo "$(date) - [$P] $1" >> "$LOG"; }

while true; do
  log "Tick"

  GAMES=$(mosquitto_sub -h "$BROKER" -t game/complete -C 1 -W 2)
  [[ "$GAMES" =~ ^[0-9]+$ ]] || GAMES=0
  (( GAMES >= 100 )) && { log "Done. 100 games played."; break; }

  TURN=$(mosquitto_sub -h "$BROKER" -t game/turn -C 1 -W 2)
  [[ "$TURN" == "$P" ]] || { log "Not my turn"; sleep 5; continue; }

  BOARD=$(mosquitto_sub -h "$BROKER" -t game/board -C 1 -W 2)
  [[ ${#BOARD} -eq 9 ]] || { log "Bad board: '$BOARD'"; sleep 5; continue; }

  readarray -t OPEN < <(for i in {0..8}; do [[ "${BOARD:$i:1}" == " " ]] && echo $i; done)
  [[ ${#OPEN[@]} -eq 0 ]] && { log "No moves."; sleep 5; continue; }

  PICK="${P}${OPEN[RANDOM % ${#OPEN[@]}]}"
  mosquitto_pub -h "$BROKER" -t game/move -m "$PICK"
  log "Played: $PICK"

  sleep 5
done
