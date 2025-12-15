#include "gltf_loader.h"

#include <gl_utility.h>
#include <glad/gl.h>

namespace gltf
{
    static void processNode(Model &model, cgltf_data *data, cgltf_node *gltfNode);

    bool loadGltf(Model &model, std::filesystem::path path)
    {
        cgltf_options options = {};
        cgltf_data *data = NULL;
        if (cgltf_parse_file(&options, path.c_str(), &data) != cgltf_result_success)
            return false;

        if (cgltf_load_buffers(&options, data, path.c_str()) != cgltf_result_success)
            return false;

        auto root = data->scene;

        for (size_t i = 0; i < root->nodes_count; i++) {
            processNode(model, data, root->nodes[i]);
        }

        model.setupBuffers();

        cgltf_free(data);
        return true;
    }

    static void processNode(Model &model, cgltf_data *data, cgltf_node *gltfNode)
    {
        for (size_t i = 0; i < gltfNode->mesh->primitives_count; i++) {
            Mesh mesh = {};
            auto primitive = gltfNode->mesh->primitives[i];

            size_t             vertexSize = primitive.attributes[0].data->count;
            std::vector<float> temp(vertexSize * 4);

            // load vertices
            mesh.vertices.resize(vertexSize);
            if (auto accessor = cgltf_find_accessor(&primitive, cgltf_attribute_type_position, 0); accessor) {
                cgltf_accessor_unpack_floats(accessor, &temp[0], vertexSize * 3);

                for (size_t j = 0; j < vertexSize; j++) {
                    mesh.vertices[j].pos.x = temp[j * 3 + 0];
                    mesh.vertices[j].pos.y = temp[j * 3 + 1];
                    mesh.vertices[j].pos.z = temp[j * 3 + 2];
                }
            }

            if (auto accessor = cgltf_find_accessor(&primitive, cgltf_attribute_type_normal, 0); accessor) {
                cgltf_accessor_unpack_floats(accessor, &temp[0], vertexSize * 3);

                for (size_t j = 0; j < vertexSize; j++) {
                    mesh.vertices[j].normal.x = temp[j * 3 + 0];
                    mesh.vertices[j].normal.y = temp[j * 3 + 1];
                    mesh.vertices[j].normal.z = temp[j * 3 + 2];
                }
            }

            if (auto accessor = cgltf_find_accessor(&primitive, cgltf_attribute_type_texcoord, 0); accessor) {
                cgltf_accessor_unpack_floats(accessor, &temp[0], vertexSize * 2);

                for (size_t j = 0; j < vertexSize; j++) {
                    mesh.vertices[j].uv.x = temp[j * 2 + 0];
                    mesh.vertices[j].uv.y = temp[j * 2 + 1];
                }
            }

            // load indices
            mesh.indices.resize(primitive.indices->count);
            cgltf_accessor_unpack_indices(primitive.indices, mesh.indices.data(), 4, mesh.indices.size());

            // load materials
            if (primitive.material) {
                if (primitive.material->has_pbr_metallic_roughness) {
                    if (primitive.material->pbr_metallic_roughness.base_color_texture.texture) {
                        const char *uri = primitive.material->pbr_metallic_roughness.base_color_texture.texture->image->uri;
                        mesh.material.baseColorTexture = gl::createTextureFromFile(uri);
                    }

                    // if (primitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture) {
                    //     const char *uri = primitive.material->pbr_metallic_roughness.metallic_roughness_texture.texture->image->uri;
                    //     mesh.material.metallicRoughnessTex = gl::createTextureFromFile(uri);
                    // }

                    // mesh.material.baseColorFactor = glm::make_vec4(primitive.material->pbr_metallic_roughness.base_color_factor);
                    // mesh.material.metallicFactor = primitive.material->pbr_metallic_roughness.metallic_factor;
                    // mesh.material.roughnessFactor = primitive.material->pbr_metallic_roughness.roughness_factor;
                }

                // if (primitive.material->normal_texture.texture) {
                //     const char *uri = primitive.material->normal_texture.texture->image->uri;
                //     mesh.material.normalTex = gl::createTextureFromFile(uri);
                // }

                // if (primitive.material->emissive_texture.texture) {
                //     const char *uri = primitive.material->emissive_texture.texture->image->uri;
                //     mesh.material.emissiveTex = gl::createTextureFromFile(uri);
                // }
            }

            model.addMesh(mesh);
        }

        for (size_t i = 0; i < gltfNode->children_count; i++) {
            processNode(model, data, gltfNode->children[i]);
        }
    }
} // namespace gltf