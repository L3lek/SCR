if [ $# != 4 ]; then
	echo "Liczba argumentów jest różna od 4."
    echo "Wpisz argumenty w kolejności: <katalog> <rozszerzenie> <liczba dni> <nazwa archiwum>"
    exit 1
fi

katalog=$1
rozszerzenie=$2
liczba_dni=$3
archiwum=$4

if [ ! -d $katalog ]; then
    echo "Nie ma takiego katalogu"
    exit 1
fi

find "$katalog" -name "*.$rozszerzenie" -mtime -$liczba_dni -readable -exec tar -rvf "$archiwum" {} \;
echo "Archiwum '$archiwum' zostało utworzone z plików o rozszerzeniu .$rozszerzenie zmodyfikowanych w ciągu ostatnich $liczba_dni dni."