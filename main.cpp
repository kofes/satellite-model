#include <iostream>
#include <sstream>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "LinearAlgebra.h"
#include "helpers/helpers.h"
#include "shader/shader.h"
#include "shader/shader_program.h"
#include <math_models/math_models.h>
#include "mvp/camera/camera.h"
#include <math_models/Forces/Forces.h>
#include "shapes/shapes.h"
#include <ctime>

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
using namespace helper;

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
    serialize::map::args(argc, argv)
            .handle("width", [&] (const serialize::values& values,
                                  const helper::serialize::ERR_CODE& err) {
                int tmp = 0;
                for (const string& value: values) {
                    stringstream sstream(value);
                    sstream >> tmp;
                    if (tmp > 0) break;
                }
                if (tmp > 0) width = tmp;
            })
            .handle("height", [&] (const serialize::values& values,
                                   const helper::serialize::ERR_CODE& err) {
                int tmp = 0;
                for (const string& value: values) {
                    stringstream sstream(value);
                    sstream >> tmp;
                    if (tmp > 0) break;
                }
                if (tmp > 0) height = tmp;
            })
            .handle("title", [&] (const serialize::values& values,
                                  const helper::serialize::ERR_CODE& err) {
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
                .add(glsl::shader::shader(GL_VERTEX_SHADER, ifstream("src/glsl/vertex.glsl")).compile())
                .add(glsl::shader::shader(GL_FRAGMENT_SHADER, ifstream("src/glsl/fragment.glsl")).compile())
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

int init_geometry();

void reshape_handler(int width, int height) {
    glViewport(0, 0, width, height);
    camera.as(width/(float)height);
}

int renderer(int argc, char* argv[]) {
    int errCode =
            init(argc, argv) ||
            init_geometry();

    if (errCode) return errCode;

    glutReshapeFunc(reshape_handler);

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
        case '+': keymap['+'] = true; break;
        case '-': keymap['-'] = true; break;
        case 'h': keymap['h'] = true; break;
        case 'l': keymap['l'] = true; break;
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
        case '+': keymap['+'] = false; break;
        case '-': keymap['-'] = false; break;
        case 'h': keymap['h'] = false; break;
        case 'l': keymap['l'] = false; break;
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

static std::shared_ptr<math::model::SatelliteOrbit> v_satOrbit;
static std::list<std::shared_ptr<glsl::object>> v_objects;

// DEBUG
helper::container::KeplerParameters v_physObjectKeplerParameters;
static double v_physObjectRotationSpeedx = 0.1;
static double v_physObjectRotationSpeedy = 0.1;
static double v_physObjectRotationSpeedz = 0.1;
static linear_algebra::Vector v_physObjectParams {
    0,  //Omega
    0,  // i
    0   // omega
};
static phys::object v_physObject(10);
//

int init_geometry() {
    helper::container::KeplerParameters params;
    params.Omega = 45;
    params.i = 65;
    params.p = 6371e+3 + 720e+3;
    params.e = 0.01;
    params.omega = 40;
    params.tau = 0;
    // DEBUG
    v_physObjectKeplerParameters.Omega = 45;
    v_physObjectKeplerParameters.i = 45;
    v_physObjectKeplerParameters.omega = 45;
    //
    helper::container::SailParameters sailParameters;
    sailParameters.rho = 0.9;
    sailParameters.Bf = sailParameters.Bb = 2./3;
    sailParameters.s = 0.9;
    sailParameters.ef = 2;
    sailParameters.eb = 0.1;
    sailParameters.area = 400;
    sailParameters.norm = {0, 0, 1, 1};

    linear_algebra::Vector sailR = {-1, 0, 0, 1};

    math::model::Satellite sat;
    sat.sail(sailParameters, sailR);

    v_satOrbit = std::shared_ptr<math::model::SatelliteOrbit>(new math::model::SatelliteOrbit(0, false));
    v_satOrbit->satellite(sat);
    v_satOrbit->parameters(params);

    return 0;
}

double satellite_speed = 0;

void idle_handler() {
    for (const auto& pr: keymap)
        if (pr.second) switch (pr.first) {
                case 'w': camera.move( camera.target() * camera.speed()); break;
                case 's': camera.move(-camera.target() * camera.speed()); break;
                case 'a': camera.move( camera.side() * camera.speed()); break;
                case 'd': camera.move(-camera.side() * camera.speed()); break;
                case ' ': camera.move( linear_algebra::Vector {0, 0, 1} * camera.speed()); break;
                case 'c': camera.move(-linear_algebra::Vector {0, 0, 1} * camera.speed()); break;
                case '-': camera.zoom(-0.1); break;
                case '+': camera.zoom( 0.1); break;
                case 'h': satellite_speed += 10; break;
                case 'l': satellite_speed = (satellite_speed > 0 ? satellite_speed - 10 : 0); break;
                default: break;
        }
    glVertexAttrib3f(shmap["camera_position"],
                     camera.position()[0],
                     camera.position()[1],
                     camera.position()[2]);
    // if (v_physObjectParams[0] < v_physObjectKeplerParameters.Omega) {
    //     v_physObject.orientation(mvp::action::R_z(v_physObjectRotationSpeedz / 180 * M_PI) * v_physObject.orientation());
    //     v_physObjectParams[0] += v_physObjectRotationSpeedz;
    // } else if (v_physObjectParams[1] < v_physObjectKeplerParameters.i) {
    //     v_physObject.orientation(mvp::action::rotate({1, 0, 0}, v_physObjectRotationSpeedx / 180 * M_PI) * v_physObject.orientation());
    //     v_physObjectParams[1] += v_physObjectRotationSpeedx;
    // } else if (v_physObjectParams[2] < v_physObjectKeplerParameters.omega) {
    //     v_physObject.orientation(mvp::action::R_z(v_physObjectRotationSpeedz / 180 * M_PI) * v_physObject.orientation());
    //     v_physObjectParams[2] += v_physObjectRotationSpeedz;
    // }
    v_satOrbit->update(satellite_speed);
    glutPostRedisplay();
}

void render() {
    glUseProgram(program_id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // VP matrix
    linear_algebra::Matrix vp = camera.model();
    // shape::solid::sphere v_physObjectVisualiser(32, 32);
    // v_physObjectVisualiser.orientation(v_physObject.orientation());
    // v_physObjectVisualiser.update_color(helper::color(150, 140, 200));
    // v_physObjectVisualiser
    //         .vertex(shmap["obj_position"])
    //         .normal(shmap["obj_normal"])
    //         .model(shmap["model"])
    //         .mvp(shmap["mvp"])
    //         .sampler_selector(shmap["select_samplers"])
    //         .material_ambient(shmap["material_ambient"])
    //         .material_diffuse(shmap["material_diffuse"])
    //         .material_specular(shmap["material_specular"])
    //         .material_emission(shmap["material_emission"])
    //         .material_shininess(shmap["material_shininess"])
    //         .show_normals(vp)
    //         .render(vp);
    // shape::solid::sphere(32, 32)
    //         .vertex(shmap["obj_position"])
    //         .normal(shmap["obj_normal"])
    //         .model(shmap["model"])
    //         .mvp(shmap["mvp"])
    //         .sampler_selector(shmap["select_samplers"])
    //         .material_ambient(shmap["material_ambient"])
    //         .material_diffuse(shmap["material_diffuse"])
    //         .material_specular(shmap["material_specular"])
    //         .material_emission(shmap["material_emission"])
    //         .material_shininess(shmap["material_shininess"])
    //         .show_normals(vp)
    //         .render(vp);
    v_satOrbit->render(v_objects);

    for (auto object: v_objects)
        object->vertex(shmap["obj_position"])
            .normal(shmap["obj_normal"])
            .model(shmap["model"])
            .mvp(shmap["mvp"])
            .sampler_selector(shmap["select_samplers"])
            .material_ambient(shmap["material_ambient"])
            .material_diffuse(shmap["material_diffuse"])
            .material_specular(shmap["material_specular"])
            .material_emission(shmap["material_emission"])
            .material_shininess(shmap["material_shininess"])
            .show_normals(vp)
            .render(vp);

    glUseProgram(0);
    glutSwapBuffers();
}

void test0() {
    // calculate force function for visualization

    // DEBUG
    v_physObjectKeplerParameters.e = 0.01;
    v_physObjectKeplerParameters.p = 6371e+3 + 650e+3;
    v_physObjectKeplerParameters.Omega = 45;
    v_physObjectKeplerParameters.i = 45;
    v_physObjectKeplerParameters.omega = 45;
    //

    // ininitalize mass parameters
    double mainMass = helper::constant::EARTH_MASS;
    double satMass = 30;
    double mu = helper::constant::G * (satMass + mainMass);

    // initialize dynamic parameters (nu, t)
    // std::srand(std::time(nullptr));
    double epsilon = 1e-5;
    double t = 1000;
    // double t = std::rand() * 1. / RAND_MAX * 1000;
    double E = helper::orbit::E(v_physObjectKeplerParameters, mainMass, satMass, t, epsilon);
    double nu = helper::orbit::nu(v_physObjectKeplerParameters, E);
    double r = helper::orbit::r(v_physObjectKeplerParameters, E);


    // addition Kepler's parameters
    double u = v_physObjectKeplerParameters.omega / 180 * M_PI + nu;

    // initialize sail parameters
    helper::container::SailParameters params;
    params.Bf = 2./3; params.Bb = 2./3;
    params.ef = 1; params.eb = 0;
    params.rho = 0.9; params.s = 0.8;
    params.area = 400;
    linear_algebra::Vector sail_norm {-1, 0, 0}; // at linked orientation system

    // initialize solar parameters
    double lambda = 90;
    // make solar normal vector !at global orientation system!
    linear_algebra::Vector sigma =
            mvp::action::R_x(-v_physObjectKeplerParameters.i / 180 * M_PI) *
            mvp::action::R_z(-v_physObjectKeplerParameters.Omega / 180 * M_PI) *
            mvp::action::R_x(helper::constant::EARTH_ECLIPTIC) *
            mvp::action::R_z(lambda / 180 * M_PI)
            * linear_algebra::Vector {1, 0, 0, 1};

    double theta_s = std::acos(sigma[0]);
    double psi_s = std::atan2(
            sigma[0] * std::cos(u) + sigma[1] * std::sin(u),
            sigma[0] * std::sin(u) - sigma[1] * std::cos(u)
    );

    linear_algebra::Vector solar_norm {
            std::sin(psi_s) * std::sin(theta_s),
            std::cos(psi_s) * std::sin(theta_s),
            std::cos(theta_s)
    };

    // intialize main orientation system
    linear_algebra::Matrix orbital_orientation =
                mvp::action::R_z(v_physObjectKeplerParameters.Omega / 180 * M_PI) *
                mvp::action::R_x(v_physObjectKeplerParameters.i / 180 * M_PI) *
                mvp::action::R_z(v_physObjectKeplerParameters.omega / 180 * M_PI) *
                mvp::action::R_z(nu / M_PI * 180);

    // reorient sail norm to orbital orientation system
    orbital_orientation.resize(3, 0);
    sail_norm = orbital_orientation * sail_norm; // to orbital orientation system

    std::ofstream fout("output.txt");
    double d1 = 0.05, d2 = 0.05, d3 = 0.1;
    size_t N1 = 2*M_PI / d1,
           N2 = 2*M_PI / d2,
           N3 = 2*M_PI / d3;

    for (size_t i1 = 0; i1 < N1; ++i1)
    for (size_t i2 = 0; i2 < N2; ++i2)
    // for (size_t i3 = 0; i3 < N3; ++i3)
    {
        // intialize linked orientation system
        linear_algebra::Matrix satellite_orientation =
                    // mvp::action::R_x(i3*d3 - M_PI) * // gamma
                    mvp::action::R_z(i2*d2 - M_PI) * // beta
                    mvp::action::R_y(i1*d1 - M_PI);  // alpha

        // reorient sail norm to global orientation system
        satellite_orientation.resize(3, 0);
        params.norm = satellite_orientation * sail_norm; // to global orientation system
        auto force =
                     force::solar(params, solar_norm)
                   // + force::atmos(v_physObjectKeplerParameters, 4.8e-12, params.area, params.norm, mu, r, nu)
                   ;
        fout << i1*d1 - M_PI << ' '
             << i2*d2 - M_PI << ' '
             // << i3*d3 - M_PI << ' '
             <<
             (std::sin(nu)) * force[0]
             // + (2*r + (1 + r/v_physObjectKeplerParameters.p) * std::cos(nu) + v_physObjectKeplerParameters.e * r / v_physObjectKeplerParameters.p)*force[1]
             << std::endl;
    }
}

void test1() {
    math::model::SatelliteOrbit v_satOrbit(0, false);

    // DEBUG
    v_physObjectKeplerParameters.e = 0.01;
    v_physObjectKeplerParameters.p = 6371e+3 + 650e+3;
    v_physObjectKeplerParameters.Omega = 45;
    v_physObjectKeplerParameters.i = 45;
    v_physObjectKeplerParameters.omega = 45;
    //

    // initialize sail parameters
    helper::container::SailParameters params;
    params.Bf = 2./3; params.Bb = 2./3;
    params.ef = 1; params.eb = 0;
    params.rho = 0.9; params.s = 0.8;
    params.area = 400;
    linear_algebra::Vector sail_norm {-1, 0, 0}; // at linked orientation system
    linear_algebra::Vector r {-1, 0, 0};

    math::model::Satellite v_satellite;
    v_satellite.sail(params, r);

    v_satOrbit.satellite(v_satellite);
    v_satOrbit.parameters(v_physObjectKeplerParameters);
    size_t COUNT_ITERATIONS = 500;
    for (size_t i = 0; i < COUNT_ITERATIONS; ++i)
        v_satOrbit.update(1);
}

void runModel(int argc, char* argv[]) {
    std::ofstream fout("genetic-output.txt");

    bool maximization;
    satellite_speed = 2;
    double TIME_LIMIT = 0;
    serialize::map::args(argc, argv)
            .handle("dt", [&] (const serialize::values& values,
                                  const helper::serialize::ERR_CODE& err) {
                int tmp = 0;
                for (const string& value: values) {
                    stringstream sstream(value);
                    sstream >> tmp;
                    if (tmp > 0) break;
                }
                if (tmp > 0) satellite_speed = tmp;
            })
            .handle("maximization", [&] (const serialize::values& values,
                                  const helper::serialize::ERR_CODE& err) {
                bool tmp = true;
                for (const string& value: values) {
                    if (value == "true")
                        tmp = true;
                    else if (value == "false")
                        tmp = false;
                }
                maximization = tmp;
            })
            .handle("time_limit", [&] (const serialize::values& values,
                                  const helper::serialize::ERR_CODE& err) {
                int tmp = 0;
                for (const string& value: values) {
                  stringstream sstream(value);
                  sstream >> tmp;
                  if (tmp > 0) break;
                }
                if (tmp > 0) TIME_LIMIT = tmp;
            });
    init_geometry();

    double current_time = 0;

    while (true && !(TIME_LIMIT && current_time >= TIME_LIMIT)) {
        auto params = v_satOrbit->parameters();
        auto satellite = v_satOrbit->satellite();

        double E = helper::orbit::E(params, helper::constant::EARTH_MASS, satellite.mass(), current_time);
        double r = helper::orbit::r(params, E);

        // LOG:
        std::cout << "angles:"
                  << "\talpha:" << satellite.angles()[0] << '\n'
                  << "\tbeta:" << satellite.angles()[1] << '\n'
                  << "\tgamma:" << satellite.angles()[2]
                  << std::endl;
        std::cout << "parameters (" << current_time << "):" << '\n'
                  << "\tp: "<< params.p << '\n'
                  << "\te: "<< params.e << '\n'
                  << "\tomega: "<< params.omega << '\n'
                  << "\ti: "<< params.i << '\n'
                  << "\tOmega: "<< params.Omega << '\n'
                  << "\th: " << r - helper::constant::EARTH_R
                  << std::endl;

        fout << current_time << ' '
             << r - helper::constant::EARTH_R << ' '
             << satellite.angles()[0] << ' '
             << satellite.angles()[1] << ' '
             << satellite.angles()[2] << std::endl;

        v_satOrbit->update(satellite_speed, maximization);
        current_time += satellite_speed;
    }
    fout.close();
}

int main(int argc, char* argv[]) {
    // test1();
    // renderer(argc, argv);
    runModel(argc, argv);
    return 0;
}
