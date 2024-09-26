//
// Created by zgh on 2024/9/19.
//

#ifndef THREE_BODY_CHECK_GL_H
#define THREE_BODY_CHECK_GL_H

#include <iostream>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#if NDEBUG
#define CHECK_GL(x) do{ \
    (x);                   \
} while(0)
#else
#define CHECK_GL(x) do{ \
    (x);                \
    check_gl_error(__FILE__,__LINE__,#x); \
} while(0)
#endif

#define PRINT(x,...) do { \
    printf(__FILE__ ":%d: " x,__LINE__ __VA_OPT__(,)__VA_ARGS__);\
} while(0)

/*
 * 单个 # 用于将宏参数转换为字符串（字符串化）
 * 双 ## 用于连接宏参数，与字符串化无关.
 * 使用const char * 不需要给返回值动态分配内存，直接返回字符串字面量
 * 减少内存分配和拷贝操作，提高性能，尤其在频繁调用的情况下。
 *  const char* 传递字符串时，实际上只是传递了指针，不涉及数据的复制
 *  const char* 指向静态或栈上的字符串，可以避免动态开销
 */
static const char *opengl_errno_name(int err) {
    switch (err) {
#define PER_GL_ERROR(x) case GL_##x: return #x;
        PER_GL_ERROR(NO_ERROR)
        PER_GL_ERROR(INVALID_ENUM)
        PER_GL_ERROR(INVALID_VALUE)
        PER_GL_ERROR(INVALID_OPERATION)
        PER_GL_ERROR(STACK_OVERFLOW)
        PER_GL_ERROR(STACK_UNDERFLOW)
        PER_GL_ERROR(OUT_OF_MEMORY)
#undef PER_GL_ERROR  //去定义
    }
    return "unknown error";
}

static void check_gl_error(const char *filename, int lineno, const char *expr) {
    auto err = glGetError();//获取之前的错误
    if (err != GL_NO_ERROR) {
        std::cerr << filename << ":" << lineno << " : " << expr << " failed: " << opengl_errno_name(err) << '\n';
        std::terminate();
        // 是C++标准库中的一个函数，用于立即终止程序的执行。
        // 调用它会导致程序异常退出，通常不执行任何清理操作
        // 使用时需要谨慎，因为它不会提供任何错误信息或回退机制
    }
}

#endif //THREE_BODY_CHECK_GL_H
