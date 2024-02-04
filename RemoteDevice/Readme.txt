/Dev_Soft_RemoteDevice_L031
Code du RmDv, progressif. Pour l'instant, code correspondant au test de durée Rentrée 2023

/Dev_Soft_ValidationRmDv_V2
Code de validation des modules RmDv version 2.0 PCB pool



TRES IMPORTANT !!
La carte 4IS gère mal le CD : pour que ça marche bien, il faut désactiver le carrier detect au niveau du module logiciel FSK.

CONFIGURATION EXCELLENTE EN QUALITE : 9600 Bds, Carte 4IS sans CD detect.
En 38400 + CD désactivé sur la carte IS = gros pépin de transmission (25%...)
-> essayer une transmission entre deux modules pour voir si l'élec HF est plus robuste que sur la carte 4IS ?


-> Test de validation 

**** CARTE n°1 OK ****
-Portée +++++ (portail / bureau 10/10)
- Prog :OK
- Reset :OK
- LED pushPull clignotant :OK
- Transm HF : OK 9600 dans les deux sens
- Test BP callback : Non fait, bug logiciel.

**** CARTE n°2 OK ****
- Portée moyenne
- Prog :OK
- Reset :OK
- LED pushPull clignotant :OK
- Transm HF : OK 9600 dans les deux sens
- Test BP callback : Non fait, bug logiciel.

**** CARTE n°3 ****
- Portée moyenne
- Prog :OK
- Reset :OK
- LED pushPull clignotant :OK
- Transm HF : OK 9600 dans les deux sens
- Test BP callback : Non fait, bug logiciel.

**** CARTE n°4 ****
- Portée moyenne
- Prog :OK
- Reset :OK
- LED pushPull clignotant :OK
- Transm HF : OK 9600 dans les deux sens
- Test BP callback : Non fait, bug logiciel.

**** CARTE n°5 ****
- Portée moyenne
- Prog :OK
- Reset :OK
- LED pushPull clignotant :OK
- Transm HF : OK 9600 dans les deux sens
- Test BP callback : Non fait, bug logiciel.

**** CARTE n°6 ****
-Portée ++++
- Prog :OK
- Reset :OK
- LED pushPull clignotant :OK
- Transm HF : OK 9600 dans les deux sens
- Test BP callback : Non fait, bug logiciel.
