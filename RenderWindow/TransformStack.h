#ifndef _TRANSFORMSTACK_H_
#define _TRANSFORMSTACK_H_

#include "stdafx.h"

enum {
    LOCAL = 0,
    GLOBAL = 1
};

class TransformStack
{
public:
    TransformStack() :
        _mode(LOCAL),
        _stack(std::vector<std::pair<glm::vec3, glm::vec3>>({ std::make_pair(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)) }))
    {}
    TransformStack(const glm::vec3& tInit, const glm::vec3& wInit) :
        _mode(LOCAL),
        _stack(std::vector<std::pair<glm::vec3, glm::vec3>>({ std::make_pair(tInit, wInit) }))
    {}
    TransformStack(const TransformStack& copy) :
        _mode(LOCAL),
        _stack(copy._stack)
    {}
    TransformStack(const std::vector<std::pair<glm::vec3, glm::vec3>>& stack) :
        _mode(LOCAL),
        _stack(stack)
    {}

    void push() { _stack.push_back(_stack.back()); }
    void pop() { _stack.pop_back(); }

    void rotate(const glm::vec3& w);
    void translate(const glm::vec3& t);

    void preRotate(const glm::vec3& w);
    void preTranslate(const glm::vec3& t);

    glm::vec3 getTranslation() const { return _stack.back().first; }
    glm::vec3 getRotation() const { return _stack.back().second; }
    std::pair<glm::vec3, glm::vec3> getTranslationAndRotation() const { return _stack.back(); }

private:
    // first in the pair is the translation
    // second is the rotation
    std::vector<std::pair<glm::vec3,glm::vec3>> _stack;
    int _mode;
};

#endif