import os, sys, re
import json, pickle
import numpy
from scipy import linalg

proj = os.environ['PROJECT_DIR']

def files():
    for root, dirs, files in os.walk(proj):
        for file in files:
            if file.endswith('.mesh'):
                yield os.path.splitext(os.path.join(root, file))[0]

def id(*args):
    return re.sub('[^a-zA-Z0-9]', '_', '_'.join(args))

#Calculate cubic spline derivatives for vals
def derivatives(vals):
    M, K = vals.shape
    
    #Set up tridiagonal matrix
    ab = numpy.zeros((3, M))
    ab[0][1:M] = 1./3
    ab[2][0:M-1] = 1./3
    ab[1][0] = ab[1][-1] = 2./3
    ab[1][1:-1] = 4./3
    
    b2 = numpy.roll(vals, 1, axis=0)
    b3 = numpy.roll(vals, -1, axis=0)
    b2[0] = b2[1]
    b3[-1] = b3[-2]
    return linalg.solve_banded((1,1), ab, b3 - b2,
        overwrite_ab=True, overwrite_b=True, check_finite=False)

for path in files():
    
    with open(path + '.mesh', 'rb') as f:
        meshes = pickle.load(f)
    try:
        with open(path + '.json', 'r') as f:
            anims = json.load(f)
    except IOError:
        anims = {mesh_name: {} for mesh_name in meshes}


    with open(path + '.c', 'w') as c_f, open(path + '.h', 'w') as h_f:
        filename = os.path.basename(path)
        
        h_f.write('#include "types.h"\n')
        c_f.write('#include "{}"\n'.format(filename + '.h'))
        c_f.write('#include "engine.h"\n')

        for mesh_name, mesh in meshes.iteritems():
            if mesh_name not in anims:
                print 'Did not find animation data for mesh "{}" in "{}.json"'.format(mesh_name, filename)
                anims[mesh_name] = {}
        
            derivs = {}
        
            for key in mesh:
                mesh[key] = numpy.array(mesh[key]).T
            
            for _, keys in anims[mesh_name].iteritems():
                for key in keys:
                    if key not in mesh:
                        print 'Error in "{}.json": No key named "{}" in mesh "{}.mesh"'.format(filename, key, filename)
                        print 'Keys are: {}'.format(', '.join('"{}"'.format(key) for key in mesh))
                        sys.exit(1)

            #Solve for the derivatives
            for name, keys in anims[mesh_name].iteritems():
                bx = numpy.array([mesh[key][0] for key in keys])
                by = numpy.array([mesh[key][1] for key in keys])
                #Dx.shape == (len(keys), n_verts)
                Dx = derivatives(bx)
                Dy = derivatives(by)
                derivs[name] = (Dx, Dy)

            #Attributes are interleaved because according to
            #https://www.khronos.org/opengl/wiki/Vertex_Specification#Interleaved_attributes
            #this is better
            
            ident = id(filename, mesh_name)
            el_sz = 2 * 4 # 2d floats

            h_f.write('extern const mesh_t %s_mesh;\n' % ident)
            
            if anims[mesh_name]:                
                p_offsets, d_offsets = {}, {}
                
                offset = 0
                for key in mesh:
                    p_offsets[key] = offset * el_sz
                    offset += 1

                null_d_offset = offset * el_sz
                offset += 1

                for name, keys in anims[mesh_name].iteritems():
                    d_offsets[name] = {}
                    for key in keys:
                        d_offsets[name][key] = offset * el_sz
                        offset += 1

                n_offsets = offset

                for name, keys in anims[mesh_name].iteritems():
                    for i in xrange(len(keys)-1):
                        step_name = id(mesh_name, name) if len(keys) == 2 else id(mesh_name, name, str(i+1))
                        h_f.write('extern const struct anim_step %s;\n' % step_name)
                        c_f.write('const struct anim_step %s = ' % step_name)
                        
                        c_f.write('{.p_from = %d, .p_to = %d, .d_from = %d, .d_to = %d};\n' % (
                            p_offsets[keys[i]], p_offsets[keys[i+1]],
                            d_offsets[name][keys[i]], d_offsets[name][keys[i+1]],))
                            
                #Write a stationary animation for every shape key (it's free!)
                for key in mesh:
                    h_f.write('extern const struct anim_step %s;\n' % id(mesh_name, 'null', key))
                    c_f.write('const struct anim_step %s = ' % id(mesh_name, 'null', key))
            
                    c_f.write('{.p_from = %d, .p_to = %d, .d_from = %d, .d_to = %d};\n' % (
                        p_offsets[key], p_offsets[key], null_d_offset, null_d_offset,))
            else:
                n_keys = len(mesh)
                n_offsets = n_keys

            _, n_verts = mesh.itervalues().next().shape
            stride = n_offsets * el_sz
            n_bytes = stride * n_verts

            c_f.write('const mesh_t %s_mesh = &(struct mesh){\n' % ident)
            c_f.write('.size = %d,\n' % n_bytes)
            c_f.write('.stride = %d,\n' % stride)
            c_f.write('.verts = (float[]){\n')

            for i in xrange(n_verts):
                for key in mesh:
                    c_f.write('{}f, {}f,\n'.format(mesh[key][0][i], mesh[key][1][i]))
            
                if anims[mesh_name]:
                    c_f.write('0.f, 0.f,\n')
                
                for name, keys in anims[mesh_name].iteritems():
                     Dx, Dy = derivs[name]
                     for j in xrange(len(keys)):
                         c_f.write('{}f, {}f,\n'.format(Dx[j][i], Dy[j][i]))
            
            c_f.write('}};\n')
