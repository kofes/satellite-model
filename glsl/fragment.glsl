#version 330 core

/// camera
in vec3 camera_normal;
/// light
vec3 scene_ambient = vec3(0.2, 0.2, 0.2);
in vec3 light_normal;
in float light_distance;
uniform vec3 light_ambient;
uniform vec3 light_diffuse;
uniform vec3 light_specular;
uniform vec3 light_emission;
uniform float attenuation_constant;
uniform float attenuation_linear;
uniform float attenuation_quadratic;
uniform vec3 spot_normal;
uniform float spot_cutoff;
uniform float spot_exponent;
/// material
uniform vec3 material_ambient;
in vec3 material_normal;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform vec3 material_emission;
uniform float material_shininess;
/// texture
in vec2 texture_uv;
uniform sampler2D normal_sampler;
uniform sampler2D diffuse_sampler;
uniform sampler2D specular_sampler;
uniform bool select_samplers;

out vec4 out_color;

void main() {
    vec3 total_light = material_emission + scene_ambient * material_ambient;
    float attenuation;
    // must be cycle for each light
    {
        attenuation = 1.0 / (
                      attenuation_constant +
                      attenuation_linear * light_distance +
                      attenuation_quadratic * light_distance * light_distance
                      );

        /// spot light
        float spot_angle = max(0.0, dot(-light_normal, spot_normal));
        if (spot_angle < cos(radians(spot_cutoff)))
            attenuation = 0;
        else
            attenuation *= pow(spot_angle, spot_exponent);

        vec3 diffuse = material_diffuse;
        vec3 specular = material_specular;
        vec3 normal = material_normal;

        /// texture
        if (select_samplers) {
            diffuse = texture(diffuse_sampler, texture_uv).rgb;
            specular = texture(specular_sampler, texture_uv).rgb;
            normal = normalize(texture(normal_sampler, texture_uv).rgb * 2.0 - 1.0);
        }

        /// diffuse
        float clamp_diffuse = clamp(dot(material_normal, light_normal), 0, 1);
        vec3 diffuse_reflection =
             attenuation *
             light_diffuse *
             material_diffuse *
             clamp_diffuse;

        /// specular
        // Texture: change camera & reflect position in dot operation!
        float clamp_specular = clamp(dot(reflect(-light_normal, material_normal), camera_normal), 0, 1);
        vec3 specular_reflection =
             attenuation *
             light_specular *
             material_specular *
             pow(clamp_specular, material_shininess);

        total_light += diffuse_reflection + specular_reflection;
    }
    // end of cycle
    out_color = vec4(total_light, 1.0);
}
