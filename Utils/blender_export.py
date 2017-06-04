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
from math import atan2
from mathutils import Vector
from contextlib import contextmanager

@contextmanager
def triangulate(mesh):
    bm = bmesh.new()
    bm.from_mesh(mesh)
    bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)
    yield bm
    bm.free()

def id(*args):
    return re.sub('[^a-zA-Z0-9]', '_', '_'.join(args)).lower()

def write_mesh(operator, context, path):
    objects = {}
    
    for obj in bpy.data.objects:
        if obj.type != 'MESH':
            continue
        
        armature = obj.parent
        if not armature or armature.type != 'ARMATURE':
            with triangulate(obj.data) as bm:
                objects[obj.name] = {
                    'plain': True,
                    'verts': [
                    v.co[0:2] for fa in bm.faces for v in fa.verts
                ]}
            continue
            
        objects[obj.name] = {'plain': False}

        bone_names = [grp.name for grp in obj.vertex_groups]
        objects[obj.name]['num_bones'] = len(bone_names)
        bones = armature.data.bones
        pbones = armature.pose.bones
        #To find the bone farthest from the root of the tree
        bone_ranks = [len(bones[name].parent_recursive) for name in bone_names]
        
        with triangulate(obj.data) as bm:
            weights = bm.verts.layers.deform.active
        
            def vert_data(vert):
                vweights = vert[weights]
                non_tiny_keys = [k for k, v in vweights.items() if v > 1/255.]
                bone_idx = max(non_tiny_keys, key=bone_ranks.__getitem__)
                bone_name = bone_names[bone_idx]
                bone_weight = round(vweights[bone_idx] * 255.)
                parent = bones[bone_name].parent
                parent_idx = bone_names.index(parent.name)
            
                rel_coord = vert.co - bones[bone_name].matrix_local.to_translation()
            
                return rel_coord[0:2] + (bone_idx, parent_idx, bone_weight)
        
            objects[obj.name]['verts'] = [
                vert_data(v) for v in bm.verts
            ]
            
            objects[obj.name]['indices'] = [
                v.index for fa in bm.faces for v in fa.verts
            ]
        
        
        actions = {strip.action
            for nla_track in armature.animation_data.nla_tracks
            for strip in nla_track.strips} | {armature.animation_data.action}
        actions = {a for a in actions if a is not None}
        
        rest_actions = [a for a in actions if 'Rest' in a.name]
        if not rest_actions:
            operator.report({'ERROR'}, 'Object {} must have an action with "Rest" in its name'.format(obj.name))
            return {'CANCELLED'}
        rest_action = rest_actions[0]
        
        objects[obj.name]['anims'] = {}
        
        initial_arm_action = armature.animation_data.action
        initial_frame = context.scene.frame_current
        try:
            for action in actions:
                objects[obj.name]['anims'][action.name] = []
                armature.animation_data.action = rest_action
                #This appears to be needed to actually apply the rest action
                context.scene.frame_set(1)
                armature.animation_data.action = action
                start, end = action.frame_range
                for frame in range(int(start), int(end)+2, 2):
                    context.scene.frame_set(frame)
                    bone_datas = []
                    for name in bone_names:
                        #the position and scale are in object coord and the rotation is in rest bone coords
                        mat = pbones[name].matrix
                        rel_mat = mat * bones[name].matrix_local.inverted()
                        bone_datas.append(mat.to_translation()[0:2]
                            + (rel_mat.to_euler().z, mat.to_scale().y))
                    objects[obj.name]['anims'][action.name].append(bone_datas)
        finally:
            armature.animation_data.action = initial_arm_action
            context.scene.frame_set(initial_frame)
    
    path = os.path.splitext(path)[0]
    
    with open(path + '.c', 'w') as c_f, open(path + '.h', 'w') as h_f:
        c_f.write('#include "engine.h"\n\n')
        h_f.write('typedef const struct mesh* mesh_t;\n')
        h_f.write('typedef struct anim_mesh* anim_mesh_t;\n')
        h_f.write('typedef const struct animation* animation_t;\n\n')
        
        for obj_name, obj in objects.items():
            if obj['plain']:
                h_f.write('extern mesh_t {};\n'.format(id(obj_name)))
                c_f.write('mesh_t {} = &(struct mesh) {{\n'.format(id(obj_name)))
                c_f.write('.size = {},\n'.format(len(obj['verts'])))
            
                c_f.write('.verts = (float[]) {\n')
                for vert in obj['verts']:
                    c_f.write('{}, {}, \n'.format(*vert))
                c_f.write('}};\n\n')
                
                continue
            
            h_f.write('extern anim_mesh_t {};\n'.format(id(obj_name)))
            c_f.write('anim_mesh_t {} = &(struct anim_mesh) {{\n'.format(id(obj_name)))
            c_f.write('.num_verts = {},\n'.format(len(obj['verts'])))
            c_f.write('.num_inds = {},\n'.format(len(obj['indices'])))
            c_f.write('.num_bones = {},\n'.format(obj['num_bones']))
            
            c_f.write('.verts = (struct anim_vert[]) {\n')
            for vert in obj['verts']:
                c_f.write('{{{}, {}, {}, {}, {}}},\n'.format(*vert))
            c_f.write('},\n')
            
            c_f.write('.indices = (GLushort []){\n')
            c_f.write(', '.join(map(str, obj['indices'])))
            c_f.write('},\n')
            
            c_f.write('};\n\n')
            
            
            for action_name, action in obj['anims'].items():
                h_f.write('extern struct animation {};\n'.format(id(obj_name, action_name)))
                c_f.write('struct animation {} = {{\n'.format(id(obj_name, action_name)))
                c_f.write('.length = {},\n'.format(len(action)-1))
                c_f.write('.frames = (struct bone* []) {\n')
                for frame in action:
                    c_f.write('(struct bone[]) {\n')
                    for bone in frame:
                        c_f.write('{{{}, {}, {}, {}}},\n'.format(*bone))
                    c_f.write('},\n')
                c_f.write('}};\n\n')

    return {'FINISHED'}


# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper


class ExportFgm(bpy.types.Operator, ExportHelper):
    """Export a model for Fighting Game"""
    bl_idname = "export.mesh"
    bl_label = "Export Fighting Game model"

    # ExportHelper mixin class uses this
    filename_ext = ".c"

    filter_glob = bpy.props.StringProperty(
            default="*.c",
            options={'HIDDEN'},
            )

    def execute(self, context):
        return write_mesh(self, context, self.filepath)


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
