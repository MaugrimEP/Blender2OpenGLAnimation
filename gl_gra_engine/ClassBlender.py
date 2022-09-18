#exec(compile(open("/home-reseau/tmayet/annee4/4S7/IG/insa_ig_tp07/practical07/ClassBlender.py").read(), "ClassBlender.py", 'exec'))

import sys

def getDirectEnfants(parent):
    enfants = []
    for enfant in bpy.data.scenes['Scene'].objects:
        if enfant.parent == parent:
            enfants+=[enfant]
    return enfants

class WrapperBlender:

	globalParent        = set()
	Wrappers            = set()

	def __init__(self,blenderObject, blenderParent = None):

		self.obj     = blenderObject
		self.enfants = set()
		self.blenderParent = blenderParent
		blenderEnfants = getDirectEnfants(blenderObject)
		
		for blenderEnfant in blenderEnfants:
			self.enfants|={WrapperBlender(blenderEnfant,blenderObject)}

		if not blenderObject.parent:
			WrapperBlender.globalParent|={self}
			
		WrapperBlender.Wrappers|={self}


	def __eq__(self,other):
		return isinstance(other,WrapperBlender) and self.obj.name == other.obj.name

	def __hash__(self):
		return hash(self.obj.name)

	def setParentCascade(self,parent):
		self.setParent(parent)
		for enfant in self.enfants:
			enfant.setParentCascade(self)

	def setParent(self,parent):
		self.parent = parent

	def getProfondeur(self):
		if self.parent == None:
			return 0
		else:
			return 1 + self.parent.getProfondeur()

	def computeLocationCascade(self):

		self.offsetX = self.obj.location.x
		self.offsetY = self.obj.location.y
		self.offsetZ = self.obj.location.z

		if self.getProfondeur()==1 and True:

			self.offsetX += self.parent.offsetX
			self.offsetY += self.parent.offsetY
			self.offsetZ += self.parent.offsetZ
		
		for enfant in self.enfants:
			enfant.computeLocationCascade()

	def isLeaf(self):
		return len(self.enfants==0)

	def getTopParent(self):
		def topParentBlender(blenderObj):
			if blenderObj.parent==None:
				return blenderObj
			else:
				return topParentBlender(blenderObj.parent)

		return topParentBlender(self.obj)
		
	def INIT():
		for o in bpy.data.scenes['Scene'].objects:
			WrapperBlender(o)
		for gp in WrapperBlender.globalParent:
			gp.setParentCascade(None)
		for gp in WrapperBlender.globalParent:
			gp.computeLocationCascade()

print("Class structure imported")