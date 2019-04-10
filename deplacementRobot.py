# -*- coding: utf-8 -*-
"""
Created on Tue Apr 09 13:08:06 2019

@author: Utilisateur
"""

import json, ast
global ListType, ListCoordObst, ListXObst, ListYObst,ListX_obst,ListY_obst
global ListCoord_terrain,ListX_terrain,ListY_terrain,Depart, collision, CurrentX, CurrentY
global CheckX, CheckY, pasMap, FieldX, FieldY, inclusion

def getDataEnv():
    global ListX_obst,ListY_obst
    descriptionEnv = json.load(open('descriptionEnv.json'))
    descriptionEnv = ast.literal_eval(json.dumps(descriptionEnv))
    
    
    for data in descriptionEnv['obstacles']:
        ListType.append(data['type'])    
    for data in descriptionEnv['obstacles']:
        ListCoordObst.append(data['coordonnees'])    
    for i in range(len(ListCoordObst)):
        if(ListType[i] == 'polygone'):
            for j in range(len(ListCoordObst[i])):
                ListX_obst.append(ListCoordObst[i][j]['point']['x']) 
                ListY_obst.append(ListCoordObst[i][j]['point']['y'])
                ListTmpX = ListX_obst[:]
                ListTmpY = ListY_obst[:]
                del ListX_obst[:]
                del ListY_obst[:]
                ListX_obst.append(max(ListTmpX))
                ListX_obst.append(min(ListTmpX))
                ListY_obst.append(max(ListTmpY))
                ListY_obst.append(min(ListTmpY))
        elif(ListType[i] == 'cercle'):
            ListX_obst.append(ListCoordObst[i][0]['centre']['x']-ListCoordObst[i][1]['rayon']) 
            ListX_obst.append(ListCoordObst[i][0]['centre']['x']+ListCoordObst[i][1]['rayon'])              
            ListY_obst.append(ListCoordObst[i][0]['centre']['y']-ListCoordObst[i][1]['rayon'])             
            ListY_obst.append(ListCoordObst[i][0]['centre']['y']+ListCoordObst[i][1]['rayon'])                
        ListX_obst_copy = ListX_obst[:]
        ListY_obst_copy = ListY_obst[:]
        ListXObst.insert(i,ListX_obst_copy)
        ListYObst.insert(i,ListY_obst_copy)
        del ListX_obst[:]
        del ListY_obst[:]
    print(ListYObst)  
    for data in descriptionEnv['terrain_evolution']:
        ListCoord_terrain.append(data)
    for i in range(len(ListCoord_terrain)):
        ListX_terrain.append(ListCoord_terrain[i]['point']['x'])     
        ListY_terrain.append(ListCoord_terrain[i]['point']['y'])

    

def getDataEvol(): 
    evolutionBase = json.load(open('evolutionBase.json'))
    #evolutionBase = ast.literal_eval(json.dumps(evolutionBase))    
    Depart = [evolutionBase['depart']['coordonnees']['x'],evolutionBase['depart']['coordonnees']['y']]
    Depart.append(evolutionBase['depart']['angle'])
    Arrivee = [evolutionBase['arrivee']['coordonnees']['x'],evolutionBase['arrivee']['coordonnees']['y']]
    Arrivee.append(evolutionBase['arrivee']['angle'])
    PosTir = [evolutionBase['position-tir_cible']['coordonnees']['x'],evolutionBase['position-tir_cible']['coordonnees']['y']]
    PosTir.append(evolutionBase['position-tir_cible']['angle'])
    for data in evolutionBase['etapes']:
        ListEtapes.append(data['coordonnees'])
    for i in range(len(ListEtapes)):
        EtapeX.append(ListEtapes[i]['x'])
        EtapeY.append(ListEtapes[i]['y'])
        
def Mappage():    
    global inclusion
    Xmax = max(ListX_terrain)
    Xmin = min(ListX_terrain)    
    Ymax = max(ListY_terrain)
    Ymin = min(ListY_terrain)
    for i in range(Xmin,Xmax+pasMap,pasMap):
        FieldX.append(i)
    for j in range(Ymin,Ymax+pasMap,pasMap):
        FieldY.append(j)
    for i in range(len(ListType)):
        for j in range(len(FieldY)+1):
            for k in range(len(FieldX)+1):
                CheckInside(ListXObst[i][1],ListXObst[i][0],ListYObst[i][1],ListYObst[i][0],FieldX[k],FieldX[k+1],FieldY[j],FieldY[j+1])
                if (inclusion == True):
                    MapObstX.append(FieldX[k])
                    MapObstY.append(FieldY[j])
                    
            
def CheckInside(X0space,X1space,Y0space,Y1space,X0objet,X1objet,Y0objet,Y1objet):
    global inclusion
    inclusion = False   
    if(X0objet >= X0space and X1objet <= X1space and Y0objet >= Y0space and Y1objet <= Y1space):
        inclusion = True
    else:
        inclusion = False
    return inclusion
#def CheckObstacles(x,y):
#    for i in range(ListType):
#        if(ListType[i] == 'cercle'):
#            if((ListXObst[i][0]+ListXObst[i][1] >= x) and (ListXObst[i][0]-ListXObst[i][1]<= x) and (ListYObst[i][0]+ListXObst[i][1] >= y) and (ListYObst[i][1]-ListXObst[i][1] <= y)):
#                collision = True
#            else:
#                collision = False              
#        elif(ListType[i] == 'polygone'):
#                if(min(ListXObst[i]) <= x and max(ListXObst[i]) >= x and min(ListYObst[i]) <= y and max(ListYObst[i])):
#                    collision = True
#                else:
#                    collision = False
#        if (collision == True):
#            return collision

#def CalculParcours():
#    CurrentX = Depart[0]
#    CurrentY = Depart[1]
#    CheckX = CurrentX
#    CheckY = CurrentY
#    CurrentAngle = Depart[2]
#    for i in range(len(EtapeX)):
#        while(CurrentX != EtapeX[i] and CurrentY != EtapeY[i]):
#            for inc in range(CurrentX)
#            CheckObstacles(CurrentX,CurrentY)
#            if (collision == false):
                

ListType = []    
ListCoordObst = []
ListX_obst = []
ListY_obst = []
ListXObst = []
ListYObst = []
ListCoord_terrain = []
ListX_terrain = [] 
ListY_terrain = [] 
ListTmpX = []
ListTmpY = []
Depart = []
EtapeX = []
EtapeY = []
ListEtapes = []
collision = False
pasMap = 5
FieldX = []
FieldY = []
MapObstX = []
MapObstY = []
getDataEnv()
getDataEvol()
Mappage()