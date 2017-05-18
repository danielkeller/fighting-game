bl_info = {
    "name": "Add 2D bone keyframe",
    "category": "Animation",
    "version": (1, 0),
    "blender": (2, 75, 0),
    "description": "Add keyframe to selected bone suitable for 2d animation",
}

import bpy


class Add2DBoneKeyframe(bpy.types.Operator):
    """Add keyframe for 2d bone"""
    bl_idname = "anim.bone2d"
    bl_label = "Insert 2D Keyframe"
    bl_options = {'REGISTER', 'UNDO'}
    
    just_fix = bpy.props.BoolProperty(name="Just fix rotation", default=False)

    def execute(self, context):

        pbones = context.selected_pose_bones
        
        for pbone in pbones:
            #Fix the rotation
            pbone.rotation_mode = 'AXIS_ANGLE'
            t, _, _, z = pbone.rotation_axis_angle
            if z < 0:
                pbone.rotation_axis_angle = (-t, 0, 0, 1)
            
            if not self.just_fix:
                pbone.keyframe_insert(data_path='rotation_axis_angle', index=0)
                if not pbone.bone.use_connect:
                    pbone.keyframe_insert(data_path='location')

        for area in bpy.context.screen.areas:
            if area.type in ['DOPESHEET_EDITOR', 'PROPERTIES']:
                area.tag_redraw()

        return {'FINISHED'}

addon_keymaps = []

def register():
    # handle the keymap
    wm = bpy.context.window_manager
    km = wm.keyconfigs.addon.keymaps.new(name='Pose', space_type='EMPTY')
    
    kmi = km.keymap_items.new(Add2DBoneKeyframe.bl_idname, 'J', 'PRESS', shift=True)
    kmi.properties['just_fix'] = False
    
    kmi = km.keymap_items.new(Add2DBoneKeyframe.bl_idname, 'J', 'PRESS')
    kmi.properties['just_fix'] = True
    addon_keymaps = [km]
    
    bpy.utils.register_class(Add2DBoneKeyframe)


def unregister():
    for km in addon_keymaps:
        wm.keyconfigs.addon.keymaps.remove(km)
    addon_keymaps = []
    
    bpy.utils.unregister_class(Add2DBoneKeyframe)


# This allows you to run the script directly from blenders text editor
# to test the addon without having to install it.
if __name__ == "__main__":
    register()