#include "Scene.h"

void SceneManager::Push(std::unique_ptr<Scene> scene) {
    m_stack.push(std::move(scene));
}

void SceneManager::Pop() {
    if (!m_stack.empty()) m_stack.pop();
}

void SceneManager::Replace(std::unique_ptr<Scene> scene) {
    if (!m_stack.empty()) m_stack.pop();
    m_stack.push(std::move(scene));
}

void SceneManager::Clear() {
    while (!m_stack.empty()) m_stack.pop();
}

Scene* SceneManager::Current() {
    if (m_stack.empty()) return nullptr;
    return m_stack.top().get();
}
