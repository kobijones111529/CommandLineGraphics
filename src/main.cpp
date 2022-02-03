#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

std::chrono::time_point<std::chrono::system_clock> g_startTime;
const std::vector<char> g_charSet { '$','@','B','%','8','&','W','M','#','*','o','a','h','k','b','d','p','q','w','m','Z','O','0','Q','L','C','J','U','Y','X','z','c','v','u','n','x','j','f','t','r','/','\\','|','(',')','1','{','}','[',']','?','-','_','+','~','<','>','I','i','!','l',';',':','"',',','^','`','\'','.', ' ' };
const std::string g_chars = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const std::string g_charsLess = " .:-=+*#%@";

struct Vertex {
    glm::vec3 position;
    float brightness;
};

int getRows() {
#if defined __APPLE__ || defined __unix__
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return static_cast<int>(w.ws_row);
#else
    return 0;
#endif
}

int getCols() {
#if defined __APPLE__ || defined __unix__
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return static_cast<int>(w.ws_col);
#else
    return 0;
#endif
}

void setCursorPos(int x, int y) {
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

void showCursor(bool show) {
    if (show) {
        std::cout << "\e[?25h";
    }
    else {
        std::cout << "\e[?25l";
    }
}

char getChar(float brightness, const std::string& chars) {
    brightness = std::min(std::max(brightness, 0.0f), 1.0f);
    return chars.substr((int)(brightness * (chars.length() - 1))).c_str()[0];
}

char getCharLess(float brightness, const std::string& chars) {
    return chars.substr((int)(brightness * (chars.length() - 1))).c_str()[0];
}

float edgeFunction(glm::vec2& v0, glm::vec2& v1, glm::vec2& p) {
    return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

float edgeFunction(glm::vec3& v0, glm::vec3& v1, glm::vec3& p) {
    return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

void draw(std::string& image, int width, int height, const std::vector<Vertex>& vertices) {
    for (int i = 0; i < vertices.size(); i += 3) {
        glm::vec3 v0 = vertices[i].position;
        glm::vec3 v1 = vertices[i + 1].position;
        glm::vec3 v2 = vertices[i + 2].position;
        float aspect = (1.0f / 20.0f) / (1.0f / 8.0f);
        v0.y *= aspect;
        v1.y *= aspect;
        v2.y *= aspect;
        float c0 = vertices[i].brightness;
        float c1 = vertices[i + 1].brightness;
        float c2 = vertices[i + 2].brightness;
        float area = edgeFunction(v0, v1, v2);
        int minX = std::min(std::min(v0.x, v1.x), v2.x);
        int maxX = std::max(std::max(v0.x, v1.x), v2.x);
        int minY = std::min(std::min(v0.y, v1.y), v2.y);
        int maxY = std::max(std::max(v0.y, v1.y), v2.y);
        for (int y = minY; y < maxY; y++) {
            for (int x = minX; x < maxX; x++) {
                glm::vec3 p { x + 0.5f, y + 0.5f, 0.0f };
                float w0 = edgeFunction(v1, v2, p);
                float w1 = edgeFunction(v2, v0, p);
                float w2 = edgeFunction(v0, v1, p);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;
                    float c = w0 * c0 + w1 * c1 + w2 * c2;
                    image[y * width + x] = getChar(c, g_chars);
                }
            }
        }
    }
}

std::string generateImage(unsigned int width, unsigned int height) {
    std::string image;
    image.assign(width * height, ' ');

    auto nowTime = std::chrono::system_clock::now();
    auto duration = nowTime - g_startTime;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    std::vector<Vertex> vertices {
        { { 10.0f, 10.0f, 0.0f }, 0.5f },
        { { 10.0f, 50.0f, 0.0f }, 1.0f },
        { { 50.0f, 10.0f, 0.0f }, 1.0f },
    };

    glm::quat q = glm::angleAxis(ms.count() / 1000.0f, glm::vec3(0, 0, 1));
    glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, -40.0f, 0.0f));

    for (Vertex& vertex : vertices) {
        glm::vec3& pos = vertex.position;
        pos = mat * glm::vec4(pos, 1.0f);
        pos = q * pos;
        pos = glm::inverse(mat) * glm::vec4(pos, 1.0f);
    }

    draw(image, width, height, vertices);

    return image;
}

void mainLoop() {
    std::cout << std::endl;
    while (true) {
        int rows = getRows();
        int cols = getCols();
        std::string image = generateImage(cols, rows);
        
        std::cout << image;
        setCursorPos(0, 0);
    }
}

void onInterrup(int signum) {
    showCursor(true);
    setCursorPos(0, getRows());
    std::cout << std::endl;
    exit(1);
}

int main() {
    signal(SIGINT, onInterrup);

    g_startTime = std::chrono::system_clock::now();
    showCursor(true);
    mainLoop();
    showCursor(true);
}