#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>

// ============================================================
// Scene 抽象基类
// ============================================================
class Scene {
public:
    virtual ~Scene() = default;

    virtual void HandleInput(const sf::RenderWindow& window) = 0;
    virtual void HandleEvent(const sf::Event& event) { (void)event; }
    virtual void Update(float dt) = 0;
    virtual void Draw(sf::RenderWindow& window) = 0;

    virtual int  GetNextScene()  const { return m_nextScene; }
    virtual int  GetEntryParam() const { return m_entryParam; }
    virtual int  GetBGMId()      const { return m_bgmId; }
    virtual bool ShouldPop()     const { return false; }
    virtual void Reset()               { m_nextScene = -1; m_entryParam = 0; }

protected:
    int  m_bgmId      = -1;
    int  m_nextScene  = -1;
    int  m_entryParam = 0;
};

// ============================================================
// SceneManager — 场景栈
// ============================================================
class SceneManager {
public:
    void Push(std::unique_ptr<Scene> scene);
    void Pop();
    void Replace(std::unique_ptr<Scene> scene);
    void Clear();

    Scene* Current();
    bool   IsEmpty() const { return m_stack.empty(); }

private:
    std::stack<std::unique_ptr<Scene>> m_stack;
};
