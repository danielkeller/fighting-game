bl_info = {
    "name": "Export Fighting Game",
    "author": "Dan Keller",
    "version": (1, 0),
    "blender": (2, 75, 0),
    "location": "File > Export",
    "description": "Export .c file for fighting game",
    "warning": "",
    "wiki_url": "",  
    "tracker_url": "",  
    "category": "Import-Export"}

import bpy, bmesh, re, os

def id(*args):
    return re.sub('[^a-zA-Z0-9]', '_', '_'.join(args))

def write_some_data(context, c_path):
    h_path = os.path.splitext(c_path)[0] + '.h'
    
    with open(c_path, 'w', encoding='utf-8') as f, open(h_path, 'w', encoding='utf-8') as h_f:
        
        for obj in bpy.data.objects:
            if obj.type != 'MESH':
                continue
            bm = bmesh.new()
            bm.from_mesh(obj.data)
            bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)
            
            n_verts = len(bm.faces) * 3
            n_keys = max(1, len(bm.verts.layers.shape))
            n_floats = n_verts * n_keys * 2
            el_sz = 2 * 4 # 2d floats
            
            h_f.write('static const GLsizei {}_stride = {};\n'.format(id(obj.name), n_keys * el_sz))
            
            for sk_n, num in zip(bm.verts.layers.shape.keys(), range(n_keys)):
                h_f.write('static const GLsizei {} = {};\n'.format(id(obj.name, sk_n), num * el_sz))
                          
            h_f.write('extern float {}_verts[{}];\n'.format(id(obj.name), n_floats))
            f.write('float {}_verts[{}] = {{\n'.format(id(obj.name), n_floats))
            for fa in bm.faces:
                for v in fa.verts:
                    if bm.verts.layers.shape:
                        for sk_n in bm.verts.layers.shape.keys():
                            sk = bm.verts.layers.shape[sk_n]
                            f.write('{}, {},\n'.format(float.hex(v[sk].x), float.hex(v[sk].y)))
                    else:
                        f.write('{}, {},\n'.format(float.hex(v.co.x), float.hex(v.co.y)))
            f.write('};\n\n')
            bm.free()
    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper


class ExportFgm(bpy.types.Operator, ExportHelper):
    """Export a model for Fighting Game"""
    bl_idname = "export.c"
    bl_label = "Export Fighting Game model"

    # ExportHelper mixin class uses this
    filename_ext = ".c"

    filter_glob = bpy.props.StringProperty(
            default="*.c",
            options={'HIDDEN'},
            )

    def execute(self, context):
        return write_some_data(context, self.filepath)


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
