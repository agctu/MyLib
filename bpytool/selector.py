import bpy
'Use a css selector like method to get the blend objects'

__author__='agctXY'
_debug=False
if __name__=='__main__':
    _debug=True
D=bpy.data
C=bpy.context
class S:
    def __init__(self,s):
        'begin with the blender object type then '
        __spliter='# .'
        self.type_=''
        self.id_=''
        self.name=''
        pos=[]
        for i in __spliter:
            if(s.find(i)!=-1):
                pos.append(s.find(i))
        pos.append(len(s))
        pos.sort()
        self.type_=s[:pos[0]]
        for i in range(0,len(pos)-1):
            if s[pos[i]]=='#':
                self.name=s[pos[i]+1:pos[i+1]]
            elif s[pos[i]]=='.':
                self.id_=int(s[pos[i]+1:pos[i+1]])
    def get(self):
        if self.name=='' and self.id_=='':
            return findElementsByType(self.type_)
        elif self.id_!='':
            return findElementsByType(self.type_)[self.id_]
        elif self.name!='':
            return findElementsByType(self.type_)[self.name]
def findElementsByType(type_):
    if type_=='obj':
        return D.objects
    elif type_=='lit':
        return D.lights
    elif type_=='mesh':
        return D.meshes
    elif type_=='scene':
        return D.scenes
    else:
        pass

if _debug:
    S('obj.2')