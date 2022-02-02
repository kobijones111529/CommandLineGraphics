#include "glm/gtc/quaternion.hpp"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"

#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

std::chrono::time_point<std::chrono::system_clock> startTime;
const std::vector<char> charSet { '$','@','B','%','8','&','W','M','#','*','o','a','h','k','b','d','p','q','w','m','Z','O','0','Q','L','C','J','U','Y','X','z','c','v','u','n','x','j','f','t','r','/','\\','|','(',')','1','{','}','[',']','?','-','_','+','~','<','>','I','i','!','l',';',':','"',',','^','`','\'','.', ' ' };
const std::string chars = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const std::string charsLess = " .:-=+*#%@";


int getRows() {
#ifdef __unix__
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return static_cast<int>(w.ws_row);
#else
    return 0;
#endif
}

int getCols() {
#ifdef __unix__
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

char getChar(float brightness) {
    brightness = std::max(std::min(brightness, 0.0f), 1.0f);
    return chars.substr((int)(brightness * (chars.length() - 1))).c_str()[0];
}

char getCharLess(float brightness) {
    return charsLess.substr((int)(brightness * (charsLess.length() - 1))).c_str()[0];
}

float edgeFunction(glm::vec2& v0, glm::vec2& v1, glm::vec2& p) {
    return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

float edgeFunction(glm::vec3& v0, glm::vec3& v1, glm::vec3& p) {
    return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

void draw(std::string& image, int width, int height, const std::vector<glm::vec3>& vertices) {
    for (int i = 0; i < vertices.size(); i += 3) {
        glm::vec3 v0 = vertices[i];
        glm::vec3 v1 = vertices[i + 1];
        glm::vec3 v2 = vertices[i + 2];
        float area = edgeFunction(v0, v1, v2);
        int minX = std::min(std::min(v0.x, v1.x), v2.x);
        int maxX = std::max(std::max(v0.x, v1.x), v2.x);
        int minY = std::min(std::min(v0.y, v1.y), v2.y);
        int maxY = std::max(std::max(v0.y, v1.y), v2.y);
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                glm::vec3 p { x + 0.5f, y + 0.5f, 0.0f };
                float w0 = edgeFunction(v1, v2, p);
                float w1 = edgeFunction(v2, v0, p);
                float w2 = edgeFunction(v0, v1, p);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;
                    float c = w0 * 0.6 + w1 * 1 + w2 * 1;
                    image[y * width + x] = getChar(c);
                }
            }
        }
    }
}

std::string generateImage(unsigned int width, unsigned int height) {
    std::string image;
    image.assign(width * height, ' ');

    auto nowTime = std::chrono::system_clock::now();
    auto duration = nowTime - startTime;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    glm::vec3 v0 = { 20, 20, 0 };
    glm::vec3 v1 = { tan(ms.count() / 1000.0f * 50) * 15 + 20, 40, 0 };
    glm::vec3 v2 = { 80, tan(ms.count() / 1000.0f * 0.7 * 50) * 15 + 10, 0 };

    glm::vec3 v3 = { 10, 10, 0 };
    glm::vec3 v4 = { 90, 10, 0 };
    glm::vec3 v5 = { 10, 80, 0 };

    float c0 = 0.1;
    float c1 = sin(ms.count() / 1000.0f * 0.63 + 0.3) / 2.0f + 0.5f;
    float c2 = sin(ms.count() / 1000.0f) / 2.0f + 0.5f;

    std::vector<glm::vec3> vertices {
        // { (cos(ms.count() / 1000.0f) / 2.0f + 0.5f) * 10 + 10, (sin(ms.count() / 1000.0f) / 2.0f + 0.5f) * 10 + 10, 0 },
        // { (cos(ms.count() / 1000.0f) / 2.0f + 0.5f) * 10 + 10, (sin(ms.count() / 1000.0f) / 2.0f + 0.5f) * 10 + 20, 0 },
        // { (cos(ms.count() / 1000.0f) / 2.0f + 0.5f) * 10 + 20, (sin(ms.count() / 1000.0f) / 2.0f + 0.5f) * 10 + 10, 0 },
        { 10, 10, 0 },
        { 10, 50, 0 },
        { 50, 10, 0 },
        
    };

    glm::quat q = glm::angleAxis(ms.count() / 1000.0f, glm::vec3(0, 0, 1));

    glm::mat4 mat = glm::translate(glm::mat4(1.0f), glm::vec3(-40.0f, -40.0f, 0.0f));
    vertices[0] = mat * glm::vec4(vertices[0], 1.0f);
    vertices[1] = mat * glm::vec4(vertices[1], 1.0f);
    vertices[2] = mat * glm::vec4(vertices[2], 1.0f);
    vertices[0] = vertices[0] * q;
    vertices[1] = vertices[1] * q;
    vertices[2] = vertices[2] * q;
    vertices[0] = glm::inverse(mat) * glm::vec4(vertices[0], 1.0f);
    vertices[1] = glm::inverse(mat) * glm::vec4(vertices[1], 1.0f);
    vertices[2] = glm::inverse(mat) * glm::vec4(vertices[2], 1.0f);
    
    std::cout << vertices[0].x;

    draw(image, width, height, vertices);

/*
    float area = edgeFunction(v0, v1, v2);


    for (int t = 0; t < nTris; t++) {
        for (uint32_t y = 0; y < height; y++) {
            for (uint32_t x = 0; x < width; x++) {
                glm::vec2 p = { x + 0.5f, y + 0.5f };
                float w0 = edgeFunction(v1, v2, p);
                float w1 = edgeFunction(v2, v0, p);
                float w2 = edgeFunction(v0, v1, p);
                if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                    w0 /= area;
                    w1 /= area;
                    w2 /= area;
                    float c = w0 * c0 + w1 * c1 + w2 * c2;
                    int pos = y * width + x;
                    image[pos] = getChar(c);
                }
            }
        }
    }
    */
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

int main() {
    startTime = std::chrono::system_clock::now();
    showCursor(true);
    mainLoop();
    showCursor(true);
}