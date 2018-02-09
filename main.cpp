#include <iostream>
#include <sstream>
#include "GL/glew.h"
#include "GL/freeglut.h"

#include "args_serializer.h"

#include "shader/shader.h"
#include "shader/shader_program.h"

#include "LinearAlgebra.h"

#include "geometric_object/grid.h"

#include "mvp/camera/camera.h"
#include <memory>

#define MAIN_LOG

#ifdef MAIN_LOG
#define LOGout (std::cout << "Main log: ")
#else
#define LOGout (std::ostream(0))
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

// DEBUG
static linear_algebra::Matrix prev_camera;

int main(int argc, char* argv[]) {
    serialize::args(argc, argv)
    .handle("argument", [&] (const serialize::values& values, const string& error) {
        for (const string& value: values)
            cout << value << endl;
    })
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
    glEnable(GL_CULL_FACE);
    glEnable(GL_LESS);

    glewExperimental = GL_TRUE;
    GLenum error_code = glewInit();
    if (error_code != GLEW_OK) {
        cout << "Error: " << glewGetErrorString(error_code) << endl;
        return 1;
    }
    cout << "Status: using GLEW " << glewGetString(GLEW_VERSION) << endl;

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glViewport(0, 0, width, height);

    camera = mvp::Camera(75, width/height, 0.5, 100);

    prev_camera = camera.model();

    try {
        glsl::shader::program program;
        program_id = program
            .add(glsl::shader::shader(GL_VERTEX_SHADER, ifstream("glsl/vertex.glsl")).compile())
            .add(glsl::shader::shader(GL_FRAGMENT_SHADER, ifstream("glsl/fragment.glsl")).compile())
            .link()
            // vertex shader
            ("mvp", glsl::shader::field::uniform, [&](GLint id) {shmap["mvp"] = id;})
            ("model", glsl::shader::field::uniform, [&](GLint id) {shmap["model"] = id;})
            ("camera_position", glsl::shader::field::attribute, [&](GLint id) {shmap["camera_position"] = id;})
            ("light_position", glsl::shader::field::attribute, [&](GLint id) {shmap["light_position"] = id;})
            ("obj_position", glsl::shader::field::attribute, [&](GLint id) {shmap["obj_position"] = id;})
            ("obj_normal", glsl::shader::field::attribute, [&](GLint id) {shmap["obj_normal"] = id;})
            ("obj_tangent", glsl::shader::field::attribute, [&](GLint id) {shmap["obj_tangent"] = id;})
            ("obj_bitangent", glsl::shader::field::attribute, [&](GLint id) {shmap["obj_bitangent"] = id;})
            ("obj_texture", glsl::shader::field::attribute, [&](GLint id) {shmap["obj_texture"] = id;})
            // fragment shader
            ("light_ambient", glsl::shader::field::uniform, [&](GLint id) {shmap["light_ambient"] = id;})
            ("light_diffuse", glsl::shader::field::uniform, [&](GLint id) {shmap["light_diffuse"] = id;})
            ("light_specular", glsl::shader::field::uniform, [&](GLint id) {shmap["light_specular"] = id;})
            ("light_emission", glsl::shader::field::uniform, [&](GLint id) {shmap["light_emission"] = id;})
            ("attenuation_constant", glsl::shader::field::uniform, [&](GLint id) {shmap["attenuation_constant"] = id;})
            ("attenuation_linear", glsl::shader::field::uniform, [&](GLint id) {shmap["attenuation_linear"] = id;})
            ("attenuation_quadratic", glsl::shader::field::uniform, [&](GLint id) {shmap["attenuation_quadratic"] = id;})
            ("spot_normal", glsl::shader::field::uniform, [&](GLint id) {shmap["spot_normal"] = id;})
            ("spot_cutoff", glsl::shader::field::uniform, [&](GLint id) {shmap["spot_cutoff"] = id;})
            ("spot_exponent", glsl::shader::field::uniform, [&](GLint id) {shmap["spot_exponent"] = id;})
            ("material_ambient", glsl::shader::field::uniform, [&](GLint id) {shmap["material_ambient"] = id;})
            ("material_diffuse", glsl::shader::field::uniform, [&](GLint id) {shmap["material_diffuse"] = id;})
            ("material_specular", glsl::shader::field::uniform, [&](GLint id) {shmap["material_specular"] = id;})
            ("material_emission", glsl::shader::field::uniform, [&](GLint id) {shmap["material_emission"] = id;})
            ("material_shininess", glsl::shader::field::uniform, [&](GLint id) {shmap["material_shininess"] = id;})
            ("normal_sampler", glsl::shader::field::uniform, [&](GLint id) {shmap["normal_sampler"] = id;})
            ("diffuse_sampler", glsl::shader::field::uniform, [&](GLint id) {shmap["diffuse_sampler"] = id;})
            ("specular_sampler", glsl::shader::field::uniform, [&](GLint id) {shmap["specular_sampler"] = id;})
            ("select_samplers", glsl::shader::field::uniform, [&](GLint id) {shmap["select_samplers"] = id;})
            .id();
    } catch (std::exception& e) {
        cout << "Exception: " << e.what() << endl;
        return 1;
    }

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
        default: break;
    }
}
void keyboard_up_handler(unsigned char button, int x, int y) {
    switch (button) {
        case 'w': keymap['w'] = false; break;
        case 's': keymap['s'] = false; break;
        case 'd': keymap['d'] = false; break;
        case 'a': keymap['a'] = false; break;
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
    if (mouse_button == GLUT_LEFT_BUTTON) {
        camera
            .yaw((x - mouse_pos[0]) * 0.1f)
            .pitch((y - mouse_pos[1]) * 0.1f);
        mouse_pos[0] = x;
        mouse_pos[1] = y;
    }
}

void idle_handler() {
    for (const std::pair<int, bool>& pr: keymap)
        if (pr.second) switch (pr.first) {
                case 'w': camera.move(-camera.target() * camera.speed()); break;
                case 's': camera.move( camera.target() * camera.speed()); break;
                case 'd': camera.move(-camera.side() * camera.speed()); break;
                case 'a': camera.move( camera.side() * camera.speed()); break;
                case ' ': camera.move( camera.up() * camera.speed()); break;
                case 'c': camera.move( camera.up() * camera.speed()); break;
                default: break;
        }
    glVertexAttrib3f(shmap["camera_position"],
                     camera.position()[0],
                     camera.position()[1],
                     camera.position()[2]);
    glutPostRedisplay();
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program_id);
    // VP matrix
    linear_algebra::Matrix vp = camera.model();
    if (vp != prev_camera)
        cout << "PV:\n" << vp << endl;
    float buff[16] = {0};
// Grid
    geometry::grid grid(20);
    GLuint vao = grid.vertex(shmap["obj_position"]).render().vao();

    glBindVertexArray(vao);

    glUniform1i(shmap["select_samplers"], false);

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            buff[i*4 + j] = vp[i][j];
    glUniformMatrix4fv(shmap["mvp"], 1, GL_FALSE, buff);
    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j < 4; ++j)
            buff[i*4 + j] = i == j ? 1.f : 0.f;
    glUniformMatrix4fv(shmap["model"], 1, GL_FALSE, buff);

    glUniform3f(shmap["material_ambient"], 0, .5, .7);
    glUniform3f(shmap["material_emissive"], .5, .5, .5);
    glUniform3f(shmap["material_specular"], 1.f, 1.f, 1.f);
    glUniform1f(shmap["material_shininess"], 3.f);
    glDrawArrays(GL_LINES, 0, 3 * 2 * 20 * 2);
    glBindVertexArray(0);

    glutSwapBuffers();
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
