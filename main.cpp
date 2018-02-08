#include <iostream>
#include <sstream>
#include "GL/glew.h"
#include "GL/freeglut.h"

#include "args_serializer.h"

using namespace std;

void idle_handler() {}
void keyboard_handler(unsigned char button, int x, int y) {}
void mouse_handler(int button, int state, int x, int y) {}
void mouse_motion_handler(int x, int y) {}

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSwapBuffers();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

int main(int argc, char* argv[]) {
    int width = 640;
    int height = 480;
    string title = "main";

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

    glutDisplayFunc(render);
    glutIdleFunc(idle_handler);
    glutKeyboardFunc(keyboard_handler);
    glutMouseFunc(mouse_handler);
    glutMotionFunc(mouse_motion_handler);

    glutMainLoop();

    glUseProgram(0);

    return 0;
}