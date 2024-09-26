#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "check_gl.h"
#include <array>
#include <random>

////----------------先执行-------------------再判断
////NDEBUG: cmake自带的宏 -DNDEBUG
//#if NDEBUG
//#define CHECK_GL(x) do{ \
//    (x);                   \
//} while(0)
//#else
//#define CHECK_GL(x) do{ \
//    (x);                \
//    check_gl_error(__FILE__,__LINE__,#x); \
//} while(0)
//#endif
//
//#define ASSERT(x) do{ \
//    if(!(x)){           \
//        std::cerr<<"Assert Failed: "<<#x<<"\n"; \
//        std::terminate();                  \
//    }\
//} while(0)
//
//#define PRINT(x,...) do { \
//    printf(__FILE__ ":%d: " x,__LINE__ __VA_OPT__(,)__VA_ARGS__);\
//} while(0)
//
//void check_gl_error(const char *filename, int lineno, const char *expr);

struct Star {
    glm::vec2 pos;
    glm::vec2 vel; //速度
};
/*
 * 类型别名
 * 命名空间引入
 * 模板别名
 */
using StarArray=std::array<Star, 4>;
StarArray stars;
static const float fade = 10.0f;
const float G = 1.0f;
const float bounce = 1.0f;
const float friction = 0.0f;

void init() {
    CHECK_GL(glEnable(GL_POINT_SMOOTH));
    CHECK_GL(glEnable(GL_BLEND)); //启用混合功能，用于实现透明效果，使得源颜色和目标颜色根据指定的混合因子进行融合
    //用于设置混合模式，控制如何将源颜色（新绘制的颜色）与目标颜色（已有的颜色）混合。
    //GL_SRC_ALPHA：表示源颜色的 alpha 值，决定了源颜色的透明度。
    //GL_ONE_MINUS_SRC_ALPHA：表示目标颜色的 alpha 值的补值（1-α），用于确定目标颜色的贡献。
    CHECK_GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    CHECK_GL(glPointSize(32.0f));
    stars[0].pos = {-0.5f, -0.5f};
    stars[0].vel = {0.1f, 0.1f};
    stars[1].pos = {0.5f, -0.5f};
    stars[1].vel = {-0.1f, 0.1f};
    stars[2].pos = {0.0f, 0.5f};
    stars[2].vel = {0.2f, 0.1f};
}
void render(float dt) {
    CHECK_GL(glColor4f(0.0f, 0.0f, 0.1f, 1.0f - std::exp(-fade * dt)));
    CHECK_GL(glRectf(-1.0f, -1.0f, 1.0f, 1.0f));
    glBegin(GL_POINTS);
    glColor4f(0.9f, 0.8f, 0.7f, 1.0f);
    for (size_t i = 0; i < stars.size(); i++) {
        glVertex3d(stars[i].pos.x, stars[i].pos.y, 0.0f);
    }
    CHECK_GL(glEnd());
}

float energy() {
    auto potE = 0.0f;
    auto movE = 0.0f;
    for (size_t i = 0; i < stars.size(); i++) {
        for (size_t j = 0; j < stars.size(); j++) {
            if (i == j) continue;
            auto p1 = stars[i].pos;
            auto p2 = stars[j].pos;
            potE += G / glm::length(p2 - p1);
        }
        auto v=glm::length(stars[i].vel);
        movE+=0.5f*v*v;
    }
    return movE-potE;
}
/*!
 * & 表示一个引用。引用是对变量的别名，允许你直接访问该变量而不需要复制它
 * 计算导数
 * decltype(+stars[i].vel) 获取 stars[i].vel 的类型
 * () 调用该类型的默认构造函数，以确保 acc 被初始化为一个合适的零值
 * @param stars
 * @param d_stars stars的导数
 */
void derivative(StarArray const &stars,StarArray &d_stars){
    for (size_t i = 0; i < stars.size(); i++) {
        auto acc= decltype(+stars[i].vel)();
        for (size_t j = 0; j < stars.size(); j++) {
            if (i == j) continue;
            auto p1 = stars[i].pos;
            auto p2 = stars[j].pos;
            auto r12 = p2 - p1;
            auto rlen = std::max(0.5f,glm::length(r12));
            auto rnorm = r12 / rlen;
            auto force = G / (rlen * rlen);
//            if (rlen<0.05f) force=-force;
            acc += force * rnorm;
        }
        d_stars[i].vel=acc;
        d_stars[i].pos=stars[i].vel;
    }
}
/*!
 * 积分
 * @param out_stars
 * @param stars
 * @param d_stars
 * @param dt
 */
void integrate(StarArray &out_stars,StarArray const & stars,StarArray const & d_stars,float dt){
    for (size_t i = 0; i < stars.size(); i++) {
        out_stars[i].vel=d_stars[i].vel*dt+stars[i].vel;
        out_stars[i].pos=d_stars[i].pos*dt+stars[i].pos;
    }
}

void integrate(StarArray &stars, StarArray const &d_stars, float dt) {
    for (size_t i = 0; i < stars.size(); i++) {
        stars[i].pos += d_stars[i].pos * dt;
        stars[i].vel += d_stars[i].vel * dt;
    }
}

void fixbounds(StarArray & stars){
    for (size_t i = 0; i < stars.size(); i++) {
        if (stars[i].pos.y < -1.0f && stars[i].vel.y < 0.0f
            || stars[i].pos.y > 1.0f && stars[i].vel.y > 0.0f) {
            stars[i].vel.y = -bounce * stars[i].vel.y;
        }
        if (stars[i].pos.x < -1.0f && stars[i].vel.x < 0.0f
            || stars[i].pos.x > 1.0f && stars[i].vel.x > 0.0f) {
            stars[i].vel.x = -bounce * stars[i].vel.x;
        }
    }
}

/*
void substep(float dt) {
    for (size_t i = 0; i < stars.size(); i++) {
        stars[i].pos += stars[i].vel * dt;
    }
    for (size_t i = 0; i < stars.size(); i++) {
        stars[i].vel.y -= 1.0f * dt;
    }
    for (size_t i = 0; i < stars.size(); i++) {
        for (size_t j = 0; j < stars.size(); j++) {
            if (i == j) continue;
            auto p1 = stars[i].pos;
            auto p2 = stars[j].pos;
            auto r12 = p2 - p1;
            auto rlen = std::max(1e-2f,glm::length(r12));
            auto rnorm = r12 / rlen;
            auto force = G / (rlen * rlen);
            auto acc = force * rnorm;
            stars[i].vel += acc * dt;
        }
    }
    for (size_t i = 0; i < stars.size(); i++) {
        if (stars[i].pos.y < -1.0f && stars[i].vel.y < 0.0f
            || stars[i].pos.y > 1.0f && stars[i].vel.y > 0.0f) {
            stars[i].vel.y = -bounce * stars[i].vel.y;
        }
        if (stars[i].pos.x < -1.0f && stars[i].vel.x < 0.0f
            || stars[i].pos.x > 1.0f && stars[i].vel.x > 0.0f) {
            stars[i].vel.x = -bounce * stars[i].vel.x;
        }
        stars[i].vel *= std::exp(-friction * dt);
    }
}
*/

void substep_rk4(float dt) {
    StarArray tmp;
    StarArray k1, k2, k3, k4, k;
    derivative(stars, k1);                  // k1 = f(y)
    integrate(tmp, stars, k1, dt / 2.0f);   // tmp = y + k1/2 dt
    derivative(tmp, k2);                    // k2 = f(tmp)
    integrate(tmp, stars, k2, dt / 2.0f);   // tmp = y + k2/2 dt
    derivative(tmp, k3);                    // k3 = f(tmp)
    integrate(tmp, stars, k3, dt);          // tmp = y + k3 dt
    derivative(tmp, k4);                    // k4 = f(tmp)
    k = StarArray();                        // k = 0
    integrate(k, k1, 1.0f / 6.0f);          // k = k + k1/6
    integrate(k, k2, 1.0f / 3.0f);          // k = k + k2/3
    integrate(k, k3, 1.0f / 3.0f);          // k = k + k2/3
    integrate(k, k4, 1.0f / 6.0f);          // k = k + k2/6
    integrate(stars, k, dt);
    fixbounds(stars);
}

void advance(float dt)
{
    size_t n=100;
    for (int i = 0; i < n; ++i) {
        substep_rk4(dt/n);
    }
    PRINT("energy: %f\n",energy());
}

int main() {
    if (!glfwInit()) {
        return -1;
    }
    GLFWmonitor *monitor=glfwGetPrimaryMonitor();
    const GLFWvidmode *mode= glfwGetVideoMode(monitor);
    GLFWwindow *window = glfwCreateWindow(mode->width, mode->height, "ThreeBody", nullptr, nullptr);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    //将指定的窗口 window 设为当前的 OpenGL 上下文。
    glfwMakeContextCurrent(window);
//    glfwSwapInterval(1); // 设置为60帧
    init();
    double t0 = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
//        CHECK_GL(glClear(GL_COLOR_BUFFER_BIT));
        double t1 = glfwGetTime();
        render((float) (t1 - t0));
        advance((float) (t1 - t0));
        t0 = t1;
        //交换前后缓冲区的内容，显示当前渲染的图像
        glfwSwapBuffers(window);
        //处理窗口的事件，如键盘输入、鼠标移动、窗口大小变化等
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
