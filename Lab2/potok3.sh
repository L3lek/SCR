if [ ! -p myfifo1 ]; then
    mkfifo myfifo1
fi

while true; do
    if read otrzymana_liczba < myfifo1; then
        echo "Otrzymana losowa liczba: $otrzymana_liczba"
    fi
    sleep 1
done