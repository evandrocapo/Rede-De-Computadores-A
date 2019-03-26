#!/bin/bash
porta=5332
echo "Executando"
gnome-terminal -- "program.sh"
gnome-terminal -- ./servidor $porta

