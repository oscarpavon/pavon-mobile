#/bin/bash
AA="glslc"

"$AA" shader_vk_tri.frag -o frag.spv
"$AA" blue.frag -o blue_frag.spv
"$AA" green.frag -o green_frag.spv
"$AA" other.vert -o other_vert.spv
"$AA" diffuse_vk.frag -o diffuse_frag.spv
"$AA" diffuse.vert -o diffuse_vert.spv
"$AA" skinned.vert -o skinned.spv
"$AA" shader_vk_uniform.vert -o uniform.spv
"$AA" in_position.vert -o in_position.spv
"$AA" shader_vk_tri.vert -o vert.spv

echo "[OK]Shaders compiled"
