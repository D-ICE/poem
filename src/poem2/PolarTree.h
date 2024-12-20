//
// Created by frongere on 20/12/24.
//

#ifndef POEM_POLARTREE_H
#define POEM_POLARTREE_H

#include <memory>
#include <list>
#include <filesystem>

#include <poem/exceptions.h>

namespace fs = std::filesystem;

using namespace poem;

namespace poem2 {

  std::string clean_name(const std::string &str) {
    std::string s = str;
    for (size_t i = 0; i < s.size(); i++) {
      if (s[i] == ' ') {
        s[i] = '_';
        continue;
      }
      if (s[i] == '_') continue;

      if (s[i] < 'A' || s[i] > 'Z' && s[i] < 'a' || s[i] > 'z') {
        s.erase(i, 1);
        i--;
      }
    }
    if (str != s) {
      spdlog::warn("Name \"{}\" has been changed in \"{}\"", str, s);
    }

    return s;
  }


  class Component {

   public:

    explicit Component(const std::string &name) :
        m_parent(nullptr),
        m_name(clean_name(name)),
        m_is_root(false) {
    }

    Component(const std::string &name, Component *parent) :
        m_name(clean_name(name)), m_parent(parent) {}

    bool is_root() const { return m_is_root; }

    void set_parent(Component *component) { m_parent = component; }

    Component *parent() const { return m_parent; }

    bool has_child() const { return !m_children.empty(); }

    fs::path address() const {

      if (m_is_root) {
        return "/";
      }

      auto left = m_parent->address();
      return left / m_name;

    }

    virtual void attach(std::shared_ptr<Component> component) {
      m_children.push_back(component);
      component->set_parent(this);
    }

    virtual void remove(std::shared_ptr<Component> component) { m_children.remove(component); }

    std::shared_ptr<Component> new_child(const std::string &name) {
      auto child = std::make_shared<Component>(name);
      attach(child);
      return child;
    }

   private:
    std::string m_name;
    bool m_is_root;
    Component *m_parent;
    std::list<std::shared_ptr<Component>> m_children;

    friend std::shared_ptr<Component> new_root();
  };


  std::shared_ptr<Component> make_node(const std::string &name) {
    return std::make_shared<Component>(name);
  }

  std::shared_ptr<Component> new_root() {
    auto root = make_node("root");
    root->m_is_root = true;
    return root;
  }


}  // poem

#endif //POEM_POLARTREE_H
