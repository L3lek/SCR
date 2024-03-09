trap 'echo "Sygnał SIGFPE zignorowany"' FPE
trap 'echo "WINDOWS CHANGE - redraw"' WINCH

sigusr1_received="false"

trap 'if [ "$sigusr1_received" = "false" ]; then
    echo "Odebrano sygnał SIGUSR1 - Czy na pewno chcesz wyłączyć? (Oczekiwanie na drugi sygnał)"
    sigusr1_received="true"
    sleep 5
    if [ "$sigusr1_received" = "true" ]; then
      sigusr1_received="false"
      echo "Czas oczekiwania na drugi sygnał minął. Kontynuowanie pracy projektora."
    fi
  else
    echo "Projektor zostanie wyłączony"
    exit 0
  fi' USR1

while true; do
  sleep 1
done