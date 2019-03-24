#!/bin/bash
gcc TCP-Servidor-Fork.c -o servidor
gcc TCP-Servidor-Sleep.c -o servidor-sleep
gcc TCP-Cliente-Basico-1.c -o cliente
echo "Compilado com sucesso"

