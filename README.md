# AmigoBots
Algorithmme de commande des AmigoBots avec ARIA.

# Objectif
Au cours d'un projet universitaire centré sur le thême de l'industrie 4.0 nous avons dû piloter 2 AmigoBots afin de ramener une pièce jusqu'à l'entrée d'un convoyeur, puis d'aller la rechercher une fois traitée.

# Performances
Aujourd'hui, nous utilisons les DirectMotion (méthodes de bas niveaux permettant de commander les vitesse linéaire & vitesse de rotation du robot) en parrallèle des ArAction (structure de haut niveau permettant de déplacer le robot jusqu'à une position [x, y]).

Ainsi, en fonction du déplacement à effectuer, nous pouvons bénéficier des avantages des 2 méthodes :
- Simplicité des ArAction
- Précision des DirectMotion

# Problèmes

- Parfois, le goal définit par l'ArAction n'est pas atteint (car très/trop proche). Cela entraîne un problème de comportement du robot qui ne parvient pas à rejoindre la position souhaitée malgré de nombreuses tentatives...

- Les obstacles non frontaux ne sont pas détectés convenablement par les sonars, de fait l'évitement d'obstacle ne se déclenche pas.

# Lien
- Video : https://www.youtube.com/watch?v=rojsx2aYrjs
