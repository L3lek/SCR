#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Użycie: $0 <PID_projektora>"
  exit 1
fi

projektor_pid="$1"

while true; do
  echo "Wybierz opcję:"
  echo "1. Wyślij SIGFPE"
  echo "2. Wyślij SIGWINCH"
  echo "3. Wyślij SIGUSR1 (wymaga dwukrotnego wysłania)"
  echo "4. Zakończ"

  read opcja
  case $opcja in
    1)
      kill -s FPE $projektor_pid
      ;;
    2)
      kill -s WINCH $projektor_pid
      ;;
    3)
      kill -s USR1 $projektor_pid
      ;;
    4)
      exit 0
      ;;
    *)
      echo "Nieznana opcja."
      ;;
  esac
done

done