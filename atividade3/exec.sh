#!/bin/bash
porta=5332
echo "Executando"
gnome-terminal -- ./servidor $porta

sh program.sh
sh program2.sh
