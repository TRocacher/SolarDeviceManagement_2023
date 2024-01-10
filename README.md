Ce dépôt, en construction, détaille la conception d'un système de pilotage de climatiseurs en fonction de l'excédent de puissance photovoltaïque pour une maison.
Le système s'appuie sur de l'existant :
- Climatiseurs (4) de type Mitsubishi 2kW
- un installation photovoltaïque avec un onduleur Fronius 6kVA SnapInverter associé à un smartMeter triphasé en point d'entrée de réseau ENEDIS

Le système va récupérer les informations de puissance produites par le SmartMeter (via l'onduleur et l'API REST Fournie par Fronius - req HTTP -  ) grâce à une 
carte Raspberry pi pour savoir si de la puissance est disponible.
Un réseau de capteurs est fabriqué (Hardware sous KiCad + Logiciel STM32L41 et STM32F103). Les capteurs communiquent en FSK 433MHz. Ces capteurs sont en fait
des Composants qui jouent le rôle de télécommande Infra Rouge afin de pouvoir commander chaque climatiseur et qui mesurent la température ambiante. Les codes IR
ont été obtenus en sniffant le signal transmis par les télécommandes d'origine.
Une passerelle est également construite, permettant d'échanger avec  les composants distants. Cette passerelle communique en UART avec la Raspberry pi. Cette dernière
hébergera un programme en Python avec une IHM sur écran tactile.
L'ensemble du projet est en cours de conception et réalisation, documenté en SysML pour avoir une vue d'ensemble (Caméo Modeler Magic Draw)
Mots clés : Logiciel embarqué C, Conception électronique, Routage KiCad, SysmL, Réseau de capteur, domotique,  ...
