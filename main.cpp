#include <iostream>
#include <sstream>
#include <math_model/Orbit.h>
#include <geometric_object/solids/solids.h>
#include <geometric_object/triangle.h>
#include "GL/glew.h"
#include "GL/freeglut.h"

#include "args_serializer.h"

#include "shader/shader.h"
#include "shader/shader_program.h"

#include "LinearAlgebra.h"

#include "geometric_object/grid.h"

#include "mvp/camera/camera.h"

#define MAIN_LOG
#define MAIN_DBG

#ifdef MAIN_LOG
#define LOGout (std::cout << "Main log: ")
#else
#define LOGout (std::ostream(0))
#endif

#ifdef MAIN_DBG
#define DBGout (std::cout << "Main debug: ")
void MAIN_DBG_MessageCallback(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam
) {
    fprintf(stderr, "OpenGL DEBUG[%s]:\n\ttype = 0x%x,\n\tseverity = 0x%x,\n\tmessage = %s\n",
             (type == GL_DEBUG_TYPE_ERROR ? "ERROR" : "LOG"), type, severity, message);
}
#else
#define DBGout (std::ostream(0))
#endif

using namespace std;

void idle_handler();
void keyboard_handler(unsigned char button, int x, int y);
void keyboard_up_handler(unsigned char button, int x, int y);
void mouse_handler(int button, int state, int x, int y);
void mouse_motion_handler(int x, int y);
void render();

static int width = 640;
static int height = 480;
static string title = "main";

static GLuint program_id;
static mvp::Camera camera;
static std::map<std::string, GLuint> shmap;
static std::map<int, bool> keymap;

int init(int argc, char* argv[]) {
    serialize::args(argc, argv)
            .handle("width", [&] (const serialize::values& values, const string& error) {
                int tmp = 0;
                for (const string& value: values) {
                    stringstream sstream(value);
                    sstream >> tmp;
                    if (tmp > 0) break;
                }
                if (tmp > 0) width = tmp;
            })
            .handle("height", [&] (const serialize::values& values, const string& error) {
                int tmp = 0;
                for (const string& value: values) {
                    stringstream sstream(value);
                    sstream >> tmp;
                    if (tmp > 0) break;
                }
                if (tmp > 0) height = tmp;
            })
            .handle("title", [&] (const serialize::values& values, const string& error) {
                title = values.empty() ? title : values.front();
            });

    // GLUT initialization
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(1000, 100);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow(title.c_str());

    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
    glEnable(GL_LESS);

    glewExperimental = GL_TRUE;
    GLenum error_code = glewInit();
    if (error_code != GLEW_OK) {
        LOGout << "Error: " << glewGetErrorString(error_code) << endl;
        return 1;
    }

    LOGout << "Status: using GLEW " << glewGetString(GLEW_VERSION) << endl;

#ifdef MAIN_DBG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC)MAIN_DBG_MessageCallback, nullptr);
#endif

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glViewport(0, 0, width, height);

    camera = mvp::Camera(75, width/(float)height, 0.5, 100);

    try {
        glsl::shader::program program;
        program_id = program
            // 'make' shader program
                .add(glsl::shader::shader(GL_VERTEX_SHADER, ifstream("glsl/vertex.glsl")).compile())
                .add(glsl::shader::shader(GL_FRAGMENT_SHADER, ifstream("glsl/fragment.glsl")).compile())
                .link()
            // [vertex shader]: link attributes & uniforms
                .link("mvp", glsl::shader::field::uniform, shmap["mvp"])
                .link("model", glsl::shader::field::uniform, shmap["model"])
                .link("camera_position", glsl::shader::field::attribute, shmap["camera_position"])
                .link("light_position", glsl::shader::field::attribute, shmap["light_position"])
                .link("obj_position", glsl::shader::field::attribute, shmap["obj_position"])
                .link("obj_normal", glsl::shader::field::attribute, shmap["obj_normal"])
                .link("obj_tangent", glsl::shader::field::attribute, shmap["obj_tangent"])
                .link("obj_bitangent", glsl::shader::field::attribute, shmap["obj_bitangent"])
                .link("obj_texture", glsl::shader::field::attribute, shmap["obj_texture"])
            // [fragment shader]: link attributes & uniforms
                .link("light_diffuse", glsl::shader::field::uniform, shmap["light_diffuse"])
                .link("light_specular", glsl::shader::field::uniform, shmap["light_specular"])
                .link("attenuation_constant", glsl::shader::field::uniform, shmap["attenuation_constant"])
                .link("attenuation_linear", glsl::shader::field::uniform, shmap["attenuation_linear"])
                .link("attenuation_quadratic", glsl::shader::field::uniform, shmap["attenuation_quadratic"])
                .link("spot_normal", glsl::shader::field::uniform, shmap["spot_normal"])
                .link("spot_cutoff", glsl::shader::field::uniform, shmap["spot_cutoff"])
                .link("spot_exponent", glsl::shader::field::uniform, shmap["spot_exponent"])
                .link("material_ambient", glsl::shader::field::uniform, shmap["material_ambient"])
                .link("material_diffuse", glsl::shader::field::uniform, shmap["material_diffuse"])
                .link("material_specular", glsl::shader::field::uniform, shmap["material_specular"])
                .link("material_emission", glsl::shader::field::uniform, shmap["material_emission"])
                .link("material_shininess", glsl::shader::field::uniform, shmap["material_shininess"])
                .link("select_samplers", glsl::shader::field::uniform, shmap["select_samplers"])
                .link("normal_sampler", glsl::shader::field::uniform, shmap["normal_sampler"])
                .link("diffuse_sampler", glsl::shader::field::uniform, shmap["diffuse_sampler"])
                .link("specular_sampler", glsl::shader::field::uniform, shmap["specular_sampler"])
            // return id of shader program
                .id();
    } catch (std::exception& e) {
        LOGout << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}

int renderer(int argc, char* argv[]) {
    init(argc, argv);

    // Bind engine functions
    glutKeyboardFunc(keyboard_handler);
    glutKeyboardUpFunc(keyboard_up_handler);

    glutMouseFunc(mouse_handler);
    glutMotionFunc(mouse_motion_handler);

    glutIdleFunc(idle_handler);
    glutDisplayFunc(render);

    // Run main
    glutMainLoop();
    return 0;
}

void keyboard_handler(unsigned char button, int x, int y) {
    switch (button) {
        case 'w': keymap['w'] = true; break;
        case 's': keymap['s'] = true; break;
        case 'd': keymap['d'] = true; break;
        case 'a': keymap['a'] = true; break;
        case ' ': keymap[' '] = true; break;
        case 'c': keymap['c'] = true; break;
        default: break;
    }
}
void keyboard_up_handler(unsigned char button, int x, int y) {
    switch (button) {
        case 'w': keymap['w'] = false; break;
        case 's': keymap['s'] = false; break;
        case 'd': keymap['d'] = false; break;
        case 'a': keymap['a'] = false; break;
        case ' ': keymap[' '] = false; break;
        case 'c': keymap['c'] = false; break;
        default: break;
    }
}

static int mouse_button;
static int mouse_pos[2];

void mouse_handler(int button, int state, int x, int y) {
    mouse_button = button;
    mouse_pos[0] = x;
    mouse_pos[1] = y;
}

void mouse_motion_handler(int x, int y) {
    if (mouse_button == GLUT_LEFT_BUTTON)
        camera
            .yaw((x - mouse_pos[0]) * 0.1f)
            .pitch((y - mouse_pos[1]) * 0.1f);
    mouse_pos[0] = x;
    mouse_pos[1] = y;
}

void idle_handler() {
    for (const auto& pr: keymap)
        if (pr.second) switch (pr.first) {
                case 'w': camera.move(-camera.target() * camera.speed()); break;
                case 's': camera.move( camera.target() * camera.speed()); break;
                case 'd': camera.move(-camera.side() * camera.speed()); break;
                case 'a': camera.move( camera.side() * camera.speed()); break;
                case ' ': camera.move(-camera.up() * camera.speed()); break;
                case 'c': camera.move( camera.up() * camera.speed()); break;
                case '-': camera.zoom(-0.1); break;
                case '+': camera.zoom( 0.1); break;
                default: break;
        }
    glVertexAttrib3f(shmap["camera_position"],
                     camera.position()[0],
                     camera.position()[1],
                     camera.position()[2]);
    glutPostRedisplay();
}

void render() {
    glUseProgram(program_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // VP matrix
    linear_algebra::Matrix vp = camera.model();

    geometry::grid(100)
            .vertex(shmap["obj_position"])
            .model(shmap["model"])
            .mvp(shmap["mvp"])
            .sampler_selector(shmap["select_samplers"])
            .material_ambient(shmap["material_ambient"])
            .material_emission(shmap["material_emission"])
            .material_shininess(shmap["material_shininess"])
            .render(vp);
    geometry::triangle({-1.5,1.5,1.5}, {-1.5,0,0}, {0,0,1.5})
            .vertex(shmap["obj_position"])
            .normal(shmap["obj_normal"])
            .model(shmap["model"])
            .mvp(shmap["mvp"])
            .sampler_selector(shmap["select_samplers"])
            .material_ambient(shmap["material_ambient"])
            .material_diffuse(shmap["material_diffuse"])
            .material_specular(shmap["material_specular"])
            .material_emission(shmap["material_emission"])
            .material_shininess(shmap["material_shininess"])
            .render(vp);

    glUseProgram(0);
    glutSwapBuffers();
}

int main(int argc, char* argv[]) {
    renderer(argc, argv);
    return 0;
}