
if [ "$1" = "-a" ]; then
  liczba_plikow=$(ls -a1 | wc -l)
else
  liczba_plikow=$(ls -1 | wc -l)
fi

echo "Liczba plików w katalogu bieżącym: $liczba_plikow"

