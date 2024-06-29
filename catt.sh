#!/bin/bash

output_file="output.txt"  # Nome del file di output
script_name="catt.sh"    # Nome dello script da escludere

# Cancella il file di output se esiste già
> "$output_file"

# Funzione ricorsiva per generare l'alberatura delle directory
generate_tree() {
  local directory="$1"
  local indent="$2"

  # Loop attraverso tutti i file e le directory nella directory corrente
  for entry in "$directory"/*; do
    # Esclude file e cartelle nascoste e lo stesso script
    if [[ $(basename "$entry") == "$script_name" || $(basename "$entry") == .* ]]; then
      continue
    fi

    if [ -d "$entry" ]; then
      echo "${indent}$(basename "$entry")/" >> "$output_file"
      generate_tree "$entry" "$indent  "
    elif [ -f "$entry" ]; then
      echo "${indent}$(basename "$entry")" >> "$output_file"
    fi
  done
}

# Funzione ricorsiva per processare i file e le directory
process_directory() {
  local directory="$1"
  local indent="$2"

  # Loop attraverso tutti i file e le directory nella directory corrente
  for entry in "$directory"/*; do
    # Esclude file e cartelle nascoste e lo stesso script
    if [[ $(basename "$entry") == "$script_name" || $(basename "$entry") == .* ]]; then
      continue
    fi

    if [ -d "$entry" ]; then
      echo "${indent}Directory: $(basename "$entry")" >> "$output_file"
      process_directory "$entry" "$indent  "
    elif [ -f "$entry" ]; then
      echo "${indent}File: $(basename "$entry")" >> "$output_file"
      echo "==> $entry <==" >> "$output_file"
      cat "$entry" >> "$output_file"
      echo -e "\n" >> "$output_file"
    fi
  done
}

# Genera l'alberatura delle directory e scrivila all'inizio del file
generate_tree "." ""

# Aggiungi una separazione tra l'alberatura e il contenuto dei file
echo -e "\n\n" >> "$output_file"

# Processa la directory corrente per il contenuto dei file
process_directory "." ""

echo "Struttura del progetto e contenuto dei file scritti in $output_file"
