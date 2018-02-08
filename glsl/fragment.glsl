#version 330 core

/// camera
in vec3 camera_normal;

/// light
// scene light
vec3 scene_ambient = vec3(0.2, 0.2, 0.2);
// light position & direction from vertex shader
in vec3 light_normal;
in float light_distance;
// light Phong specification
in vec3 light_ambient;
in vec3 light_diffuse;
in vec3 light_specular;
in vec3 light_emission;
// approximation of light intensive
in float attenuation_constant;
in float attenuation_linear;
in float attenuation_quadratic;
// spot light specification
in vec3 spot_normal;
in float spot_cutoff;
in float spot_exponent;

/// material
in vec3 material_ambient;

in vec3 material_normal;
in vec3 material_diffuse;
in vec3 material_specular;

in vec3 material_emission;
in float material_shininess;

/// texture
in vec2 texture_uv;

uniform sampler2D normal_sampler;
uniform sampler2D diffuse_sampler;
uniform sampler2D specular_sampler;

/// choose between samplers & materials
uniform bool select_samlers;

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

        /// texture
        if (select_samlers) {
            material_diffuse = texture(diffuse_sampler, texture_uv).rgb;
            material_specular = texture(specular_sampler, texture_uv).rgb;
            material_normal = normalize(texture(normal_sampler, texture_uv).rgb * 2.0 - 1.0);
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
