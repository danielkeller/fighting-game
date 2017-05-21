bl_info = {
    "name": "Export Fighting Game",
    "author": "Dan Keller",
    "version": (1, 0),
    "blender": (2, 75, 0),
    "location": "File > Export",
    "description": "Export rigged .c file for fighting game",
    "warning": "",
    "wiki_url": "",  
    "tracker_url": "",  
    "category": "Import-Export"}

import bpy, bmesh, re, os

def id(*args):
    return re.sub('[^a-zA-Z0-9]', '_', '_'.join(args)).lower()

#Calculate total rotation relative to base
def total_rotation(pbone):
    rot = 0.0
    while pbone:
        #I don't know why this has to be negative
        rot -= pbone.rotation_axis_angle[0]
        pbone = pbone.parent
    return rot

def write_mesh(context, path):
    objects = {}
    
    for obj in bpy.data.objects:
        if obj.type != 'MESH':
            continue
        
        armature = obj.parent
        if armature.type != 'ARMATURE':
            continue
        
        bones = armature.data.bones
        pbones = armature.pose.bones
        
        #To find the bone farthest from the root of the tree
        bone_names = [grp.name for grp in obj.vertex_groups]
        bone_ranks = [len(bones[name].parent_recursive) for name in bone_names]
        
        def fix_weights(weights):
            bone_idx = max(weights.keys(), key=bone_ranks.__getitem__)
            bone_weight = round(weights[bone_idx] * 255.)
            parent = bones[bone_names[bone_idx]].parent
            parent_idx = bone_names.index(parent.name)
            return bone_idx, parent_idx, bone_weight
        
        objects[obj.name] = {}
        
        bm = bmesh.new()
        bm.from_mesh(obj.data)
        bmesh.ops.triangulate(bm, faces=bm.faces[:], quad_method=0, ngon_method=0)
        
        weights = bm.verts.layers.deform.active
        objects[obj.name]['verts'] = [
            (v.co.x, v.co.y) + fix_weights(v[weights])
            for fa in bm.faces
            for v in fa.verts
        ]
        bm.free()
        
        objects[obj.name]['bones'] = [
            bones[name].matrix_local.to_translation()[0:2]
            for name in bone_names
        ]
        
        actions = {strip.action
            for nla_track in armature.animation_data.nla_tracks
            for strip in nla_track.strips} | {armature.animation_data.action}
        
        bone_rots = {name: bones[name].matrix_local.to_euler().z
            for name in bone_names}
        
        objects[obj.name]['anims'] = {}
        
        initial_arm_action = armature.animation_data.action
        initial_frame = context.scene.frame_current
        try:
            for action in actions:
                objects[obj.name]['anims'][action.name] = []
                armature.animation_data.action = action
                start, end = action.frame_range
                for frame in range(int(start), int(end)+2, 2):
                    context.scene.frame_set(frame)
                    bone_datas = []
                    for name in bone_names:
                        #the position and scale are absolute and the rotation is relative
                        bone_datas.append(pbones[name].matrix.to_translation()[0:2]
                            + (total_rotation(pbones[name]), pbones[name].matrix.to_scale().y))
                    objects[obj.name]['anims'][action.name].append(bone_datas)
        finally:
            armature.animation_data.action = initial_arm_action
            context.scene.frame_set(initial_frame)
    
    path = os.path.splitext(path)[0]
    
    with open(path + '.c', 'w') as c_f, open(path + '.h', 'w') as h_f:
        c_f.write('#include "engine.h"\n\n')
        h_f.write('typedef const struct anim_mesh* anim_mesh_t;\n')
        h_f.write('typedef const struct animation* animation_t;\n\n')
        
        for obj_name, obj in objects.items():
            h_f.write('extern anim_mesh_t {};\n'.format(id(obj_name)))
            c_f.write('anim_mesh_t {} = &(struct anim_mesh) {{\n'.format(id(obj_name)))
            c_f.write('.size = {},\n'.format(len(obj['verts'])))
            
            c_f.write('.bones = {\n')
            for bone in obj['bones']:
                c_f.write('{{{}, {}}},\n'.format(*bone))
            c_f.write('},\n')
            
            c_f.write('.verts = (struct vert[]) {\n')
            for vert in obj['verts']:
                c_f.write('{{{}, {}, {}, {}, {}}},\n'.format(*vert))
            c_f.write('}};\n\n')
            
            for action_name, action in obj['anims'].items():
                h_f.write('extern animation_t {};\n'.format(id(obj_name, action_name)))
                c_f.write('animation_t {} = &(struct animation){{\n'.format(id(obj_name, action_name)))
                c_f.write('.length = {},\n'.format(len(action)-1))
                c_f.write('.frames = (struct bone[][MAX_BONES]) {\n')
                for frame in action:
                    c_f.write('{\n')
                    for bone in frame:
                        c_f.write('{{{}, {}, {}, {}}},\n'.format(*bone))
                    c_f.write('},\n')
                c_f.write('}};\n\n')


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
