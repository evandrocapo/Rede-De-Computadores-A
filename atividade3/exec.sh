!/bin/bash
porta=5332
echo "Executando"
gnome-terminal -- ./servidor $porta

gnome-terminal -- ./cliente 127.0.0.1 $porta
gnome-terminal -- ./cliente 127.0.0.1 $porta