# Below is to convert poloygons to triangles
# f = open("SciFi_Fighter.obj", "r")
# w = open("fighter_processed.obj", "w")

# triangle_num = 0
# for line in f.readlines():
#     if line[0] == 'f':
#         parts = line.split(" ")
#         if (len(parts) == 5):
#             triangle_num += 1
#             w.write(line)
#             continue

#         triangle_num += 2
#         w.write(parts[0] + " " + parts[1] + " " + parts[2] + " " + parts[3] + "\n")
#         w.write(parts[0] + " " + parts[1] + " " + parts[3] + " " + parts[4] + "\n")
#     else :
#         w.write(line)

# w.close()

# print("Written triangle {}".format(triangle_num))


# Below is to convert negative indexing to positive indexing
# f = open("fighter_processed.obj", "r")
# w = open("fighter_processed_2.obj", "w")

# v_count, vn_count, vt_count = 0, 0, 0
# triangle_num = 0
# for line in f.readlines():
#     parts = line.split()
#     if len(parts) == 0:
#         continue
#     if parts[0] == "v":
#         v_count += 1
#         w.write(line)
#     elif parts[0] == "vn":
#         vn_count += 1
#         w.write(line)
#     elif parts[0] == "vt":
#         vt_count += 1
#         w.write(line)
#     elif parts[0] == "f":
#         w.write("f")
#         for i in [1, 2, 3]:
#             vertex = parts[i]
#             indices = vertex.split("/")
#             v_index =  int(indices[0]) + v_count + 1
#             vt_index = int(indices[1]) + vt_count + 1
#             vn_index = int(indices[2]) + vn_count + 1

#             w.write(" " + str(v_index) + "/" + str(vt_index) + "/" + str(vn_index))

#         w.write("\n")
#         triangle_num += 1
#     else:
#         w.write(line)

# w.close()
    
# print("Written triangle {}".format(triangle_num))

# Below is changing tex coord of coronavirus to make it prettier
f = open("sphere.obj", "r")
w = open("sphere_processed.obj", "w")

for line in f.readlines():
    parts = line.split()
    if len(parts) > 0 and parts[0] == "v":
        w.write("vt {} {}\n".format(parts[1], parts[2]))

    w.write(line)