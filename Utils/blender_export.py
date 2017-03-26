bl_info = {
    "name": "Export Fighting Game",
    "author": "Dan Keller",
    "version": (1, 0),
    "blender": (2, 75, 0),
    "location": "File > Export",
    "description": "Export .mesh file for fighting game",
    "warning": "",
    "wiki_url": "",  
    "tracker_url": "",  
    "category": "Import-Export"}

import bpy, bmesh, re, os, pickle

def write_mesh(context, path):
    result = {}
    
    for obj in bpy.data.objects:
        if obj.type != 'MESH':
            continue
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)

        result[obj.name] = {}
        
        if bm.verts.layers.shape:
            for sk_name in bm.verts.layers.shape.keys():
                sk = bm.verts.layers.shape[sk_name]
                result[obj.name][sk_name] = [
                    (v[sk].x, v[sk].y)
                    for fa in bm.faces
                    for v in fa.verts
                ]
        else:
            result[obj.name]['Basis'] = [
                (v.co.x, v.co.y)
                for fa in bm.faces
                for v in fa.verts
            ]

        bm.free()

    with open(path, 'wb') as f:
        pickle.dump(result, f, protocol=2)


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper


class ExportFgm(bpy.types.Operator, ExportHelper):
    """Export a model for Fighting Game"""
    bl_idname = "export.mesh"
    bl_label = "Export Fighting Game model"

    # ExportHelper mixin class uses this
    filename_ext = ".mesh"

    filter_glob = bpy.props.StringProperty(
            default="*.mesh",
            options={'HIDDEN'},
            )

    def execute(self, context):
        write_mesh(context, self.filepath)
        return {'FINISHED'}


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportFgm.bl_idname, text="Fighting Game Export")


def register():
    bpy.utils.register_class(ExportFgm)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportFgm)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()
