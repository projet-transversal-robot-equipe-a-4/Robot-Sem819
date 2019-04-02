#include "c8051F020.h"
#include <math.h>

#define PI 3.14159265358979323846

double pos_x_robot = 0;
double pos_y_robot = 0;

double pos_angulaire_h = 0;
double pos_angulaire_v = 0;

double pos_x_cible = 10;
double pos_y_cible = 10;
double pos_z_cible = 10;

/*//////////////////////////////////////////////////////////////

Avec
pos_x_robot la position en x dans le repère du robot en mm
pos_y_robot la position en y dans le repère du robot en mm

pos_angulaire_h la position angulaire horizontale du pointeur
pos_angulaire_v la position angulaire verticale du pointeur

rotation_h(angle) fonction assurant la rotation horizontale du pointeur
rotation_v(angle) fonction assurant la rotation verticale du pointeur

pos_x_cible en mm
pos_y_cible en mm
pos_z_cible en mm

/////////////////////////////////////////////////////////////*/

double pos_rel_x_cible = pos_x_cible - pos_x_robot;
double pos_rel_y_cible = pos_y_cible - pos_y_robot;
double pos_rel_z_cible = pos_z_cible - 285;

int cas_possible // à terme, 1 si la cible est dans le premier quartile à partir de la position du robot, 2 si deuxième quartile...

double theta_rad;
double phi_rad;

if (pos_rel_y_cible =< 0)
{
	cas_possible = 1;
}
else
{
	cas_possible = 0;
}

switch (cas_possible) 
{

	
	case 1 :
		theta_rad = atan(pos_rel_y_cible/pos_rel_x_cible);
		phi_rad = atan(pos_rel_z_cible/(sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y)));
		break;

	case 2 :
		theta_rad = PI - atan(pos_rel_y_cible/(-pos_rel_x_cible));
		phi_rad = atan(pos_rel_z_cible/(sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y)));
		break;
	case 3 :
		theta_rad = PI - atan((-pos_rel_y_cible)/(-pos_rel_x_cible));
		//A partir de là c'est pas bon
		phi_rad = - atan(pos_rel_z_cible/(sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y)));
		break;

	case 4 :
		theta_rad = PI - atan(pos_rel_y_cible/(-pos_rel_x_cible));
		phi_rad = atan(pos_rel_z_cible/(sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y)));
		break;
}