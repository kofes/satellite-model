#version 330 core

uniform mat4 mvp;
uniform mat4 model;

in vec3 obj_position;
in vec3 obj_normal;
in vec3 obj_tangent;
in vec3 obj_bitangent;
in vec2 obj_texture;
in vec3 light_position;
in vec3 camera_position;

out vec3 camera_normal;
out vec3 light_normal;
out float light_distance;
out vec3 material_normal;
out vec2 texture_uv;

void main() {
    gl_Position = mvp * vec4(obj_position, 1.0);

    vec3 model_position = (model * vec4(obj_position, 1.0)).xyz;

    light_normal = normalize(light_position - model_position);
    light_distance = distance(light_position, model_position);

    camera_normal = normalize(camera_position - model_position);

    texture_uv = obj_texture;

    material_normal = normalize(mat3(model) * obj_normal);
    vec3 tangent = normalize(mat3(model) * obj_tangent);
    vec3 bitangent = normalize(mat3(model) * obj_bitangent);

    mat3 TBN = transpose(mat3(
        tangent,
        bitangent,
        material_normal
    ));

    light_normal = TBN * light_normal;
    camera_normal = TBN * camera_normal;
}