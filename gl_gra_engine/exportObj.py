import bpy
import os
from math import pi
import time
import bmesh

#exec(compile(open("/home-reseau/wmocaer/S7/IG/insa_ig_tp07/practical07/exportObj.py").read(), "exportObj.py", 'exec'))

exec(compile(open("/home-reseau/wmocaer/S7/IG/insa_ig_tp07/practical07/structuralBlenderToCpp.py").read(), "structuralBlenderToCpp.py", 'exec'))
exec(compile(open("/home-reseau/wmocaer/S7/IG/insa_ig_tp07/practical07/ClassBlender.py").read(), "ClassBlender.py", 'exec'))

destfolder = "/home-reseau/wmocaer/S7/IG/insa_ig_tp07/practical07/sfmlGraphicsPipeline/meshes/"

objects = bpy.data.scenes['Scene'].objects

#main functions

def all():
    WrapperBlender.INIT();
    bakeObjects()
    exportClasses()
    exportInitialSettings()
    exportKeyFrames()

def bakeObjects():
    for ob in objects:
        print("baking "+ob.name)
        bake(ob)


#remet a O les locations de tout les mesh de la scene, mais laisse les points où ils apparaissaient
#càd translate l'origine du mesh de moins ça location, et translate tout les vertex du mesh de la location du mesh
def resetAllLocation():
    for ob in objects:
        resetLocationObject(ob)

def exportClasses():
    for parentWrappers in WrapperBlender.globalParent:
        exportToCpp(parentWrappers)

#export de la situation initial
#export tout les mesh en les translatant à la postion 0
#les paramatères de leurs positions sont dans le fichier nom_mesh.txt, ces paramètres sont destiné à être lu en opengl afin 
#de définir la matrice des transformation du mesh par rapport à son père
def exportInitialSettings():
    def rec(wob,folderName):
        exportInitialSettingsObj(wob,folderName)
        for we in wob.enfants:
            rec(we,folderName)

    for wob in WrapperBlender.globalParent:
        folderName = wob.obj.name
        rec(wob,folderName)

#export les keyframes de tout les objets
def exportKeyFrames():
    frame_rate = bpy.data.scenes['Scene'].render.fps
    frame_end = bpy.context.scene.frame_end

    for ob in objects:
        if not hasParent(ob):
            folderName = ob.name
            exportKeyFrameOfObj(ob,frame_rate,frame_end,folderName) # on export les keyFrames du pere
            for enfant in getEnfants(ob): # on export les keys frame de tout les enfants
                exportKeyFrameOfObj(enfant,frame_rate,frame_end, folderName) # on export les keyFrames du pere


#translate l'origine du mesh de moins ça location, et translate tout les vertex du mesh de la location du mesh
def resetLocationObject(ob):
    x,y,z=ob.location.x, ob.location.y, ob.location.z
    
    ob_data = ob.data
    for vert in ob_data.vertices:
        vert.co.x+=x
        vert.co.y+=y
        vert.co.z+=z

    ob.location = (0,0,0)

    return (x,y,z)
    
#fait l'opération inverse de resetLocationObjet, prend en paramatre le resultat de resetLocationObjet
def restorLocationObj(ob,rx,ry,rz):    
    ob_data = ob.data
    for vert in ob_data.vertices:
        vert.co.x-=rx
        vert.co.y-=ry
        vert.co.z-=rz

    ob.location = (rx,ry,rz)


def exportInitialSettingsObj(wob,folderName):
    ob = wob.obj
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.object.select_pattern(pattern = ob.name)


    x , y , z = 0 , 0 , 0

    x = wob.offsetX
    y = wob.offsetY
    z = wob.offsetZ    

    os.makedirs(os.path.dirname(destfolder+folderName+"/"), exist_ok=True)

    file = open(destfolder + folderName + "/" + ob.name + ".txt","w+")  
    file.write(str(x)+"\n"+str(y)+"\n"+str(z)) 
    file.close()

    oldLocation = ob.location
    #ob.location=(0,0,0)
    bpy.ops.export_scene.obj(
        filepath = destfolder + folderName+ "/" + ob.name + ".obj",
        use_selection = True,
        use_normals=True,
        keep_vertex_order=True,
        use_materials=False,
        global_scale=1,
        axis_forward='Y',
        axis_up='Z'
        ,)
                
    bpy.ops.object.select_all(action='DESELECT')

    ob.location=oldLocation


def hasParent(ob):
    return ob.parent!=None

def isEnfantOf(enfant,parent):
    if not hasParent(enfant):
        return False
        
    if enfant.parent==parent:
        return True

    return isEnfantOf(enfant.parent,parent)

def getEnfants(parent):
    enfants = []
    for enfant in objects:
        if isEnfantOf(enfant, parent):
            enfants+=[enfant]
    return enfants

#export les keyframe d'un objet en particulier
def exportKeyFrameOfObj(ob,frame_rate,frame_end,folderName):

    bpy.ops.screen.frame_jump(0)#on initialise la keyframe a la keyframe d'origine
    
    os.makedirs(os.path.dirname(destfolder+folderName+"/animation/"), exist_ok=True)
    file = open(destfolder+folderName+"/animation/" + ob.name + "_keyframes.txt","w+")  
    file.write(str(kf2S(frame_rate,frame_end))+"\n") #la durée de l'animation

    finished = {'FINISHED'}

    while {'FINISHED'} == finished: #on itérère sur les keyframes (set la keyframe à la suivant)
        
        current_frame = bpy.data.scenes['Scene'].frame_current

        xyz      =[]
        rotation =[]
        scale    =[]

        for i in range(0,len(ob.animation_data.action.fcurves)):
            
            current_data = round(ob.animation_data.action.fcurves[i].evaluate(current_frame),2)
            current_type = ob.animation_data.action.fcurves[i].data_path
            if current_type=='location':
                xyz+=[current_data]
            if current_type=='rotation_euler':
                rotation+=[current_data]
            if current_type=='scale':
                scale+=[current_data]

        file.write(str(kf2S(frame_rate,current_frame))+"\n")
        [file.write(str(x)+" ") for x in xyz]
        file.write("\n")
        [file.write(str(x)+" ") for x in rotation]
        file.write("\n")
        [file.write(str(x)+" ") for x in scale]
        file.write("\n")

        finished = bpy.ops.screen.keyframe_jump()
        
    file.close()
    

#traduit l'indice de la keyframe dans la durée actuelle où en est l'animation
def kf2S(frame_rate,keyframe): #keyframe to seconde
    return keyframe/frame_rate

def unwrap(obj):
    bpy.ops.object.mode_set(mode='OBJECT')
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.object.select_pattern(pattern = obj.name)
    bpy.context.scene.objects.active = obj
    obj.select = True

    clearUvMaps()

    bpy.ops.mesh.uv_texture_add()                                         # 
    bpy.ops.object.mode_set(mode='EDIT')                                  #
    bpy.ops.uv.smart_project( angle_limit = 66, island_margin = 0 )       #on smart uv unwrap


def bake(obj):

    bpy.ops.object.mode_set(mode='OBJECT')
    bpy.ops.object.select_all(action='DESELECT')                          #on déselectionne tout
    bpy.ops.object.select_pattern(pattern = obj.name)                      #on sélectionne l'objet
    bpy.context.scene.objects.active = obj
    obj.select = True

    clearUvMaps()
    clearImages()

    image = getImage("UV"+obj.name)                                       #on crée une nouvelle image pour sauvegarder la texture
    path = destfolder+"../textures/UV"+obj.name+".png"
    image.filepath = path
    image.file_format = 'PNG'

    
    bpy.ops.mesh.uv_texture_add()                                         # 
    bpy.ops.object.mode_set(mode='EDIT')                                  #

    bpy.ops.mesh.select_all(action='SELECT')

    bpy.ops.uv.smart_project( angle_limit = 66, island_margin = 0 )       #on smart uv unwrap
    
    bindImageToObject(obj,image)                                          #on bind l'image sur l'obj
    bpy.data.screens['UV Editing'].areas[1].spaces[0].image = image       #l'image courrant de l'uv editing devient l'image
    bpy.context.scene.render.bake_type = 'TEXTURE'                        # SETTINGS :on bake la texture
    bpy.data.scenes["Scene"].render.use_bake_selected_to_active = True    # SETTINGS : on ne travaile que sur l'object courrant
    bpy.ops.object.bake_image()                                           #on bake


    image.save()
    bpy.ops.object.mode_set(mode='OBJECT')
    
    bpy.ops.object.select_all(action='DESELECT')

    obj.select = False

def getImage(imageName):
    return bpy.data.images.new (
    name   = imageName,
    width  = 1024,
    height = 1024,
    alpha  = True,
    )

#lie limage uv a l'obj
def bindImageToObject(ob,img):

    for uv_face in ob.data.uv_textures.active.data:
        uv_face.image = img
        
def clearImages():
    for image in bpy.data.images:
        image.user_clear()
        bpy.data.images.remove(image)

def clearUvMaps():
    while bpy.ops.mesh.uv_texture_remove() != {'CANCELLED'}:
        pass
        

print("imported")


#exec(compile(open("/home-reseau/tmayet/annee4/4S7/IG/insa_ig_tp07/practical07/exportObj.py").read(), "exportObj.py", 'exec'))

#a la compilation on veut exporter les setting initiales et toutes les keyframes
##exportInitialSettings()
##exportKeyFrames()