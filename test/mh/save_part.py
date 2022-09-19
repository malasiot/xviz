import bpy

obj = bpy.data.objects['Human_cmu:Body']

with open('/home/malasiot/Downloads/oo.txt', 'w') as file:
    for f in obj.data.polygons:
        if f.select:
            file.write(str(f.index))
            file.write('\n')
