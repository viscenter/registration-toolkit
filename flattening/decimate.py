import sys
import bpy

# useful for debugging
def dump(obj):
  for attr in dir(obj):
    print("obj.%s = %s" % (attr, getattr(obj, attr)))

################ Change these values to change program behavior ################
#
# ratio of remaining faces to original faces
ratio = 0.5
#
# number of times to apply decimater of the specified ratio
iterations = 2
#
# if True, rotate the mesh 90 degrees on the X axis after decimating
rotate = True
#
################################################################################

argv = sys.argv
# process all arguments after '--'
argv = argv[argv.index('--') + 1:]

inFileName = argv[0]
outFileName = argv[1]

assert len(bpy.data.objects) == 0, \
  'you must change the default blender scene to be empty'

bpy.ops.import_scene.obj(filepath=inFileName,
                         use_split_objects=False,
                         use_split_groups=False,
                         split_mode='OFF')

# select mesh
bpy.context.scene.objects.active = bpy.context.scene.objects[0]

# move mesh to origin
bpy.ops.object.origin_set(type='GEOMETRY_ORIGIN')

# decimate
for i in range(iterations):
    bpy.ops.object.modifier_add(type='DECIMATE')
    bpy.context.object.modifiers["Decimate"].ratio = ratio
    # dump(bpy.context.object.modifiers["Decimate"]) # DEBUG
    bpy.ops.object.modifier_apply(apply_as='DATA', modifier="Decimate")

# rotate 90 degrees (for Chad dataset this was necessary to make the mesh horizontal)
if rotate:
  bpy.ops.transform.rotate(value=1.5708,   # 1.5708 rad = 90 deg
                           axis=(1, 0, 0), # X axis
                           constraint_axis=(True, False, False),
                           constraint_orientation='GLOBAL',
                           mirror=False,
                           proportional_edit_falloff='SMOOTH',
                           proportional_size=1)

bpy.ops.export_scene.obj(filepath=outFileName,
                         keep_vertex_order=True)
