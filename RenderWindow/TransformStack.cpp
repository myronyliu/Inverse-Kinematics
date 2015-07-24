#include "TransformStack.h"
#include "Math.h"

void TransformStack::rotate(const glm::vec3& w) {
    if (_mode == LOCAL) {
        glm::mat3 R0 = Math::R(_stack.back().second);
        glm::mat3 R1 = Math::R(R0*w);
        glm::mat3 R = R1*R0;
        _stack.back().second = Math::w(R);
    }
    else if (_mode == GLOBAL)
        _stack.back().second = Math::w(Math::R(w)*Math::R(_stack.back().second));
}

void TransformStack::translate(const glm::vec3& t) {
    if (_mode == LOCAL)
        _stack.back().first += Math::R(_stack.back().second)*t;
    else if (_mode == GLOBAL)
        _stack.back().first += t;
}

void TransformStack::preRotate(const glm::vec3& w) {
    if (_mode == LOCAL) {
        glm::mat3 R = Math::R(w);
        _stack.back().second = Math::w(R*Math::R(R*_stack.back().second));
        _stack.back().first = R*_stack.back().first;
    }
    else if (_mode == GLOBAL) {
        _stack.back().second = Math::w(Math::R(_stack.back().second)*Math::R(w));
    }
}

void TransformStack::preTranslate(const glm::vec3& t) {
    if (_mode == LOCAL || _mode == GLOBAL)
        _stack.back().first += t;
}