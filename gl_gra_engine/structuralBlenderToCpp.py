import bpy
import os
from math import pi
import time
import bmesh

from string import Template

#exec(compile(open("/home-reseau/tmayet/annee4/4S7/IG/insa_ig_tp07/practical07/structuralBlenderToCpp.py").read(), "structuralBlenderToCpp.py", 'exec'))


def exportToCpp(parent):
	code = """
	#ifndef HPP_$parent
	#define HPP_$parent

	#include "../HierarchicalRenderable.hpp"
	#include "../CompletMeshRenderable.hpp"
	#include "../ShaderProgram.hpp"
	#include "../Viewer.hpp"

	class Class$parent {
	public:
		void executeImport(Viewer& viewer, ShaderProgramPtr shader){


			$objectCreation
			
			$linkCreation

			$topParentAddRenderable
			
			$addAnimations
		}
	};

	#endif
	"""

	objectCreation   = buildHierarchieObj(parent)
	linkCreation     = buildHierarchieLink(parent)
	addAnimations    = "".join([setUpAnimation(parent.obj,we.obj) for we in parent.enfants|{parent}])

	classCpp = Template(code).substitute(
		parent                 = parent.obj.name,
		objectCreation         = objectCreation,
		linkCreation           = linkCreation,
		addAnimations          = addAnimations,
		topParentAddRenderable = addRenderable(parent.obj)
		)

	destfolder = "/home-reseau/wmocaer/S7/IG/insa_ig_tp07/practical07/sfmlGraphicsPipeline/include/setUpClassesMesh/"
	os.makedirs(os.path.dirname(destfolder), exist_ok=True)
	file = open(destfolder + "Class" +parent.obj.name + ".hpp","w+")
	file.write(classCpp) 
	file.close()


def buildHierarchieObj(wrapper):

	allEnfantObj = ""
	for wrapperEnfant in wrapper.enfants:
		allEnfantObj+= buildHierarchieObj(wrapperEnfant)
		
	return Template("""

		$currentObj

		$allEnfantObj

		""").substitute(
			currentObj   = createRenderable(wrapper.getTopParent(), wrapper.obj),
			allEnfantObj = allEnfantObj,					
			)

def buildHierarchieLink(wrapper):

	allEnfantLink = ""
	for wrapperEnfant in wrapper.enfants:
		allEnfantLink += addChild(wrapper.obj,wrapperEnfant.obj)
		allEnfantLink += buildHierarchieLink(wrapperEnfant)

	return allEnfantLink

def createRenderable(parent,enfant):
	#ig_dragon_uv2
	#
	return Template('auto $enfant = std::make_shared<CompletMeshRenderable>(shader,"$parent","$enfant","UV$enfant.png");\n\t\t\t').substitute(
			enfant= enfant.name,
			parent= parent.name
		)

def addChild(parent,enfant):
	return Template('HierarchicalRenderable::addChild( $parent, $enfant);\n\t\t\t').substitute(enfant= enfant.name,parent=parent.name)

def setUpAnimation(parent,enfant):
	return Template('$enfant->setUpAnimation("$parent","$enfant",viewer);\n\t\t\t').substitute(enfant= enfant.name,parent=parent.name)

def addRenderable(obj):
	return Template('viewer.addRenderable($obj);\n\t\t\t').substitute(obj=obj.name)


print("Class template imported")