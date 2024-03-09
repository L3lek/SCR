for arg in "$@"; do
    if [ -e "$arg" ]; then
        if [ -f "$arg" ]; then
            echo "'$arg' to plik istnieje"
        elif [ -d "$arg" ]; then
            if [ "$(ls -A "$arg")" ]; then
                echo "'$arg' to katalog"
            else
                echo "'$arg' to katalog pusty"
            fi
        fi
    else
        echo "'$arg' nie istnieje"
    fi
done