#include "obj_loader.h"

#include "tiny_obj_loader.h"

namespace obj
{
    bool loadObj(Model &model, std::filesystem::path path)
    {
        tinyobj::attrib_t                attrib;
        std::vector<tinyobj::shape_t>    shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());

        // if (!warn.empty()) {
        //     printf("obj load warning: %s\n", warn.c_str());
        // }

        // if (!err.empty()) {
        //     fprintf(stderr, "obj load error: %s\n", err.c_str());
        // }

        if (!ret) {
            fprintf(stderr, "Failed to load OBJ model - '%s'!\n", path.c_str());
            return false;
        }

        for (size_t s = 0; s < shapes.size(); s++) {
            tinyobj::mesh_t &objMesh = shapes[s].mesh;
            Mesh mesh = {};

            size_t index_offset = 0;
            for (size_t f = 0; f < objMesh.num_face_vertices.size(); f++) {
                size_t fv = 3;

                // Loop over vertices in the face.
                for (size_t v = 0; v < fv; v++) {
                    Vertex vertex = {};

                    const tinyobj::index_t idx = objMesh.indices[index_offset + v];

                    vertex.pos.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    vertex.pos.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    vertex.pos.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    // normal
                    if (idx.normal_index >= 0) {
                        vertex.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        vertex.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        vertex.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }

                    // uv
                    if (idx.texcoord_index >= 0) {
                        vertex.uv.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        vertex.uv.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    }

                    // color
                    // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                    // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                    // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

                    mesh.vertices.push_back(vertex);
                }
                index_offset += fv;

                // per-face material
                // const tinyobj::material_t &objMaterial = materials[objMesh.material_ids[f]];
            }

            model.addMesh(mesh);
        }

        model.setupBuffers();

        return true;
    }
}