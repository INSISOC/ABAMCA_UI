#!/bin/bash

# Configuración
CARPETA_ENTRADA="input" # Carpeta que contiene subcarpetas con archivos .txt
DIRECTORIO_SIMULACION=".."         # Directorio donde está el Makefile

# Verificar que el directorio de simulación y el Makefile existen
if [ ! -d "$CARPETA_ENTRADA" ]; then
    echo "Error: La carpeta '$CARPETA_ENTRADA' no existe."
    exit 1
fi

if [ ! -f "$DIRECTORIO_SIMULACION/Makefile" ] && [ ! -f "$DIRECTORIO_SIMULACION/makefile" ]; then
    echo "Error: No se encontró un Makefile en '$DIRECTORIO_SIMULACION'."
    exit 1
fi

# Encontrar todos los archivos .txt en subdirectorios de batch_simulation
find "$CARPETA_ENTRADA" -type f -name "*.txt" | while read INPUT_FILE; do
    # Asegurarse de que sea un archivo regular
    if [ ! -f "$INPUT_FILE" ]; then
        echo "Saltando '$INPUT_FILE', no es un archivo."
        continue
    fi

    echo "Procesando archivo: $INPUT_FILE"

    # Copiar el archivo al directorio de simulación como data_input.txt
    cp "$INPUT_FILE" "$DIRECTORIO_SIMULACION/data_auction.txt"

    # Moverse al directorio de simulación y ejecutar make all
    cd "$DIRECTORIO_SIMULACION" || exit

    make all | while read line; do
        echo "$line"  # Esto imprime las líneas que salen por pantalla (opcional)
        
        # Comprobamos si la línea contiene el mensaje final
        if [[ "$line" == *"THIS IS THE FINAL ALLOCATION"* ]]; then
            echo "La simulación ha terminado."
            break  # Salir del bucle cuando el mensaje aparece
        fi
    done

    # Verificar que se generó el archivo output.csv
    if [ ! -f "output.csv" ]; then
        echo "Error: No se generó el archivo output.csv tras la simulación."
        exit 1
    fi

    # Obtener el directorio donde se encuentra el archivo de entrada
    INPUT_DIR=$(dirname "$INPUT_FILE")

    # Mover el archivo output.csv a la carpeta de entrada
    
    mv "output.csv" "./batch_simulation/$INPUT_DIR/"

    # Generar el nombre de salida basado en el archivo de entrada
    OUTPUT_NAME=$(basename "$INPUT_FILE" | sed 's/\.[^.]*$//').csv

    # Renombrar el archivo output.csv con el nombre adecuado
    mv "./batch_simulation/$INPUT_DIR/output.csv" "./batch_simulation/$INPUT_DIR/$OUTPUT_NAME"

    # Mostrar mensaje de éxito
    echo "Resultado guardado en './batch_simulation/$INPUT_DIR/$OUTPUT_NAME'."

    # Volver al directorio original
    cd - >/dev/null
done

echo "Todas las simulaciones han sido procesadas."
