import sys
import bpy
from mathutils import Vector

# useful for debugging
def dump(obj):
    for attr in dir(obj):
        print("obj.%s = %s" % (attr, getattr(obj, attr)))
    print()

argv = sys.argv
argv = argv[argv.index('--') + 1:]

inFileName = argv[0]
outFileName = argv[1]

################################################################################
frames = 300
################################################################################

bpy.context.scene.frame_start = 1
bpy.context.scene.frame_end = frames

assert len(bpy.data.objects) == 0, \
    'you must change the default blender scene to be empty'

# bpy.ops.import_scene.obj(filepath=inFileName)
bpy.ops.import_scene.obj(filepath=inFileName,
                         use_split_objects=False,
                         use_split_groups=False,
                         split_mode='OFF')

# select mesh
mesh = bpy.context.scene.objects[0]
bpy.context.scene.objects.active = mesh

# get minimum coordinate value of each axis in mesh
# why are the coordinates ordered this way? I don't know
minX, minZ, minY = list(map(min, zip(*[list(vertex.co) for vertex in mesh.data.vertices])))

# move mesh up so that minimum coordinate is now 1 above 0 (right above collision plane)
mesh.location.z += 1 - minZ

# set physics settings on mesh
bpy.ops.object.modifier_add(type='CLOTH')

# Blender 'Silk' presets
bpy.context.object.modifiers["Cloth"].settings.quality = 5
bpy.context.object.modifiers["Cloth"].settings.mass = 0.150
bpy.context.object.modifiers["Cloth"].settings.structural_stiffness = 5
bpy.context.object.modifiers["Cloth"].settings.bending_stiffness = 0.05
bpy.context.object.modifiers["Cloth"].settings.spring_damping = 0
bpy.context.object.modifiers["Cloth"].settings.air_damping = 1

# create collision plane
bpy.ops.mesh.primitive_plane_add(radius=1,
                                 view_align=False,
                                 enter_editmode=False,
                                 location=(0, 0, 0),
                                 layers=(True, False, False, False, False, False, False, False, False, False, \
                                         False, False, False, False, False, False, False, False, False, False))

plane = bpy.context.scene.objects[0]
bpy.context.scene.objects.active = plane

# make plane big enough that page hits it
plane.scale = Vector((400, 400, 400))

# set physics settings on plane
bpy.ops.rigidbody.object_add()
bpy.context.object.rigid_body.type = 'PASSIVE'
bpy.ops.object.modifier_add(type='COLLISION')

bpy.context.scene.frame_current = frames
bpy.ops.ptcache.bake_all(bake=True)
# bpy.ops.ptcache.bake(bake=True)

bpy.ops.object.delete(use_global=False)

# bpy.ops.export_scene.obj(filepath=outFileName)
bpy.ops.export_scene.obj(filepath=outFileName,
                         keep_vertex_order=True)
