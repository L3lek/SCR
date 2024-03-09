
current_date=$(date +%w)

echo $current_date

if [ "$current_date" = 1 ] || [ "$current_date" = 2 ] || [ "$current_date" = 3 ] || [ "$current_date" = 4 ] || [ "$current_date" = 5 ]; then
  echo "Dzień roboczy. Witaj w pracy!"
else
  echo "Dzień weekendu. Miłego odpoczynku!"
fi