#version 330 core

/// camera
in vec3 camera_normal;

/// light
in vec3 light_normal;
in float light_distance;

vec3 scene_ambient = vec3(.2, .2, .2);
uniform vec3 light_diffuse;
uniform vec3 light_specular;

uniform float attenuation_constant;
uniform float attenuation_linear;
uniform float attenuation_quadratic;

uniform vec3 spot_normal;
uniform float spot_cutoff;
uniform float spot_exponent;

/// material
in vec3 material_normal;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform vec3 material_emission;
uniform float material_shininess;

/// texture
in vec2 texture_uv;

uniform bool select_samplers;

uniform sampler2D normal_sampler;
uniform sampler2D diffuse_sampler;
uniform sampler2D specular_sampler;

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

        vec3 normal = material_normal;
        vec3 diffuse = material_diffuse;
        vec3 specular = material_specular;

        /// texture
        if (select_samplers) {
            normal = normalize(texture(normal_sampler, texture_uv).rgb * 2.0 - 1.0);
            diffuse = texture(diffuse_sampler, texture_uv).rgb;
            specular = texture(specular_sampler, texture_uv).rgb;
        }

        /// diffuse
        float clamp_diffuse = clamp(dot(normal, light_normal), 0, 1);
        vec3 diffuse_reflection =
             attenuation *
             light_diffuse *
             diffuse *
             clamp_diffuse;

        /// specular
        // Texture: change camera & reflect position in dot operation!
        float clamp_specular = clamp(dot(reflect(-light_normal, normal), camera_normal), 0, 1);
        float specular_pow = 1;
        if (material_shininess >= 0 && clamp_specular > 0)
            specular_pow = pow(clamp_specular, material_shininess);
        vec3 specular_reflection =
             attenuation *
             light_specular *
             specular *
             specular_pow;

        total_light += diffuse_reflection + specular_reflection;
    }
    // end of cycle
    gl_FragColor = vec4(total_light, 1);
}
