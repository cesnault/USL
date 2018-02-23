# USL
Ceci est un prototype de mouvement que nous souhaitons implementé dans notre project,il n'est pas parfait et va etre retravaillé.


Instruction:
- ZQSD pour se déplacer
- A pour s'accroupir (Oui c'est completement bizarre,cela sera changer dansla version finale)
- espace pour sauter
-espace maintenu permet soit de grimper aux murs(face)/ de courrir sur les murs(de profil).
- si vous maintenez ctrl pendant quer vous avancez, votre personnage va glisser sur une certaines distance puis se relever ou s'accroupir si il n'y a pas de place au dessus de lui.

Si jamais rien ne marche il se peut que se soit les valeurs dans MyBaseCharacter (Blueprint) qui ont été remis à zero voici les valeurs:

Dash :
- Dash Speed : 1400
- Dash Z :0.5

Climbing :
- Climbing Speed : 500
- Climbing Projection : 700
- Hauteur grimpable maximale : 500

Course Murale :
- Wallrun Speed : 650
- Distance courable maximale : 800
- Retombe après 800 unités de longueur parcourues

Saut Mural :
- Walljump Force : 1200
- Walljump Z : 0.5

Glissade :
- Slide Speed : 600
- Freiner après 400 unités de longueur parcourues

Fichier C++ : C++ Classes/USL/BaseCharacter (Penser à générer les fichier Visual Studio)
Blueprint : Content/Blueprint/MyBaseCharacter
