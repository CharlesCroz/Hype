# Chapitre 0 - Hello World

Premier objectif : les outils, et un premier Hello World **utilisable** à partir des outils Xilinx. En particulier le GCC "qui va bien" pour pouvoir booter en JTAG ou sur carte SD.

## Projet Vivado

Zybo_GPIO_led_sws_INT_btn.xsa : Dans la PL, GPIO pour les 4 leds et les 4 switchs à l'adresse 0x4000_0000, et les 4 boutons sur le contrôlleur d'interruption [64-61].

## Projet Vitis

Creation du projet avec l'Application Hello World sur le CPU0 à partir du XSA. Compilée et testée avec succès sur Vitis.

A partir de la console Vitis, extraction des commandes tcl à exécuter pour correctement programmer la carte.
Les filtres ont été modifiés pour anonymiser notamment mes cartes (numéro de série du JTAG dans les commandes) et utiliser des chemins relatifs pour les fichiers.
Commande utile dans xsdb : targets -target-properties (pour comprendre les filtres).

Todo : Faire le tri dans les sources pour extraire "Juste ce qu'il faut". Et Mettre en place la compilation hors de Vitis.
