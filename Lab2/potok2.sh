if [ ! -p myfifo ]; then
    mkfifo myfifo
fi

if [ ! -p myfifo1 ]; then
    mkfifo myfifo1
fi

while true; do
    if read otrzymana_liczba < myfifo; then
        echo "Otrzymana losowa liczba: $otrzymana_liczba"
		zmieniona_liczba=$((otrzymana_liczba * 20))
		echo "$zmieniona_liczba" > myfifo1
    fi
    sleep 1
done