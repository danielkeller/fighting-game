import os, re

proj = os.environ['PROJECT_DIR']

output_path_c = os.path.join(proj, 'Fighting Game/shaders.c')
output_path_h = os.path.join(proj, 'Fighting Game/shaders.h')

def files():
    for root, dirs, files in os.walk(proj):
        for file in files:
            if file.endswith('.vert') or file.endswith('.frag'):
                yield os.path.join(root, file)

types = {'.vert': 'GL_VERTEX_SHADER', '.frag': 'GL_FRAGMENT_SHADER'}

with open(output_path_c, 'w') as output_c, open(output_path_h, 'w') as output_h:
    output_h.write('typedef struct shader* shader_t;\n')
    output_c.write('#include "engine.h"\n')
    
    for in_path in files():
        name = re.sub('[^a-zA-Z0-9]', '_', os.path.basename(in_path))
        type = types[os.path.splitext(in_path)[1]]
        output_h.write('extern shader_t %s;\n' % name)
        output_c.write('shader_t %s = &(struct shader){\n.shader = 0,\n' % name)
        
        output_c.write('#ifdef DEBUG\n')
        output_c.write('.fname = "%s",\n' % in_path)
        output_c.write('#endif\n')
        
        output_c.write('.name = "%s",\n' % name)
        output_c.write('.type = %s,\n' % type)
        output_c.write('.source =\n')
        with open(in_path) as source:
            for line in source.read().splitlines():
                line = line.strip()
                #if line:
                output_c.write('"%s\\n"\n' % line)
        output_c.write('};\n')
