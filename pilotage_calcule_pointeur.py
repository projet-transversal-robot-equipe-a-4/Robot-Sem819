# -*- coding: utf-8 -*-
import numpy as np
import math as m

"""
Importer les json ici
"""

pos_x_robot = pos_robot['position-tir_cible']['coordonnees']['x']
pos_y_robot = pos_robot['position-tir_cible']['coordonnees']['y']

pos_angulaire_h = 0
pos_angulaire_v = 0

pos_x_cible = pos_cible['cible']['centre']['x']
pos_y_cible = pos_cible['cible']['centre']['y']
pos_z_cible = pos_cible['cible']['hauteur']

"""///////////////////////////////////////////////////////////////

Avec
pos_x_robot la position en x dans le repère du robot en cm
pos_y_robot la position en y dans le repère du robot en cm

pos_angulaire_h la position angulaire horizontale du pointeur
pos_angulaire_v la position angulaire verticale du pointeur

rotation_h(angle) fonction assurant la rotation horizontale du pointeur
rotation_v(angle) fonction assurant la rotation verticale du pointeur

pos_x_cible en cm
pos_y_cible en cm
pos_z_cible en cm

//////////////////////////////////////////////////////////////"""

pos_rel_x_cible = pos_x_cible - pos_x_robot
pos_rel_y_cible = pos_y_cible - pos_y_robot
pos_rel_z_cible = pos_z_cible - 28.5

def quadrant1(pos_rel_x_cible,pos_rel_y_cible):  
    
  theta_rad = np.arctan(pos_rel_y_cible/pos_rel_x_cible)
  phi_rad = np.arctan(pos_rel_z_cible/(m.sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y_cible)))
  return theta_rad,phi_rad


def quadrant2(pos_rel_x_cible,pos_rel_y_cible):
		
  theta_rad = m.pi - np.arctan(pos_rel_y_cible/(-pos_rel_x_cible))
  phi_rad = np.arctan(pos_rel_z_cible/(m.sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y_cible)))
  return theta_rad,phi_rad

  
def quadrant3(pos_rel_x_cible,pos_rel_y_cible):
		
  theta_rad = np.arctan((-pos_rel_y_cible)/(-pos_rel_x_cible))
  phi_rad = m.pi - np.arctan(pos_rel_z_cible/(m.sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y_cible)))
  return theta_rad,phi_rad
    
  
def quadrant4(pos_rel_x_cible,pos_rel_y_cible):
		
  theta_rad = m.pi - np.arctan(-pos_rel_y_cible/(pos_rel_x_cible))
  phi_rad = m.pi - np.arctan(pos_rel_z_cible/(m.sqrt(pos_rel_x_cible*pos_rel_x_cible + pos_rel_y_cible*pos_rel_y_cible)))
  return theta_rad,phi_rad
  
  
if pos_rel_z_cible < 0 :
  
  print("IMPOSSIBLE")
  
elif pos_rel_y_cible >= 0 and pos_rel_x_cible >= 0 :

	theta_rad, phi_rad = quadrant1(pos_rel_x_cible,pos_rel_y_cible)

elif pos_rel_y_cible >= 0 and pos_rel_x_cible <= 0 :
  
  theta_rad, phi_rad = quadrant2(pos_rel_x_cible,pos_rel_y_cible)
  
elif pos_rel_y_cible <= 0 and pos_rel_x_cible <= 0 :
  
  theta_rad, phi_rad = quadrant3(pos_rel_x_cible,pos_rel_y_cible)
  
else :

  theta_rad, phi_rad = quadrant4(pos_rel_x_cible,pos_rel_y_cible)
  
theta_deg = 90 - m.floor(theta_rad * 180/m.pi)
phi_deg = 90 - m.floor(phi_rad * 180/m.pi)

print (theta_deg)
print (phi_deg)