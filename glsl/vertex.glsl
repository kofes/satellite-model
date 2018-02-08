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

out vec3 normal_light;
out vec3 normal_camera;
out vec3 normal;
out vec2 texture_obj;
out float distance_light;

void main() {
    gl_Position = mvp * vec4(obj_position, 1.0);

    vec3 model_position = mat3(model) * obj_position;

    distance_light = distance(light_position, model_position);
    normal_light = normalize(light_position - model_position);
    normal_camera = normalize(camera_position - model_position);

    texture_obj = obj_texture;

    normal = mat3(model) * normalize(obj_normal);
    vec3 tangent = mat3(model) * normalize(obj_tangent);
    vec3 bitangent = mat3(model) * normalize(obj_bitangent);

    mat3 TBN = transpose(mat3(
        tangent,
        bitangent,
        normal
    ));

    normal_light = TBN * normal_light;
    normal_camera = TBN * normal_camera;
}