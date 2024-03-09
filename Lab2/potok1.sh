if [ ! -p myfifo ]; then
    mkfifo myfifo
fi

while true; do
    losowa_liczba=$(shuf -i 0-10 -n 1)
    echo "$losowa_liczba" > myfifo
    sleep 1
done