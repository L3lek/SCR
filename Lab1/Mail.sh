plik_z_adresami="lista.txt"
tresc_emaila="Treść przykładowego maila"

if [ ! -f "$plik_z_adresami" ]; then
    echo "Plik '$plik_z_adresami' nie istnieje."
    exit 1
fi

while IFS= read -r adres_emaila
do
    echo "$tresc_emaila" | mail -s "Temat przykładowego maila" "$adres_emaila"

    if [ $? -eq 0 ]; then
        echo "E-mail został wysłany na adres: $adres_emaila"
    else
        echo "Błąd podczas wysyłania e-maila na adres: $adres_emaila"
    fi
done < "$plik_z_adresami"



