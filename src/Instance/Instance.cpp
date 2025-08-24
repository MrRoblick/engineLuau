#include <Instance/Instance.h>
#include <Event/Event.h>
#include <memory>
#include <string>
#include <optional>
#include <mutex>
#include <algorithm>
#include <typeinfo>
#include <vector>

InstancePtr Instance::findFirstChild(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutexChildren);
    for (const auto& c : m_children) {
        if (c && c->name == name) {
            return c;
        }
    }
    return nullptr;
}

InstancePtr Instance::findFirstChildOfClass(const std::string& className) {
    std::lock_guard<std::mutex> lock(m_mutexChildren);
    for (const auto& c : m_children) {
        if (c && c->getClassName() == className) {
            return c;
        }
    }
    return nullptr;
}

InstancePtr Instance::findFirstAncestor(const std::string& name) {
    InstancePtr cur = parent.lock();
    while (cur) {
        {
            std::lock_guard<std::mutex> lk(cur->m_mutexChildren);
            if (cur->name == name) return cur;
        }
        cur = cur->parent.lock();
    }
    return nullptr;
}

InstancePtr Instance::findFirstAncestorOfClass(const std::string& className) {
    InstancePtr cur = parent.lock();
    while (cur) {
        {
            std::lock_guard<std::mutex> lk(cur->m_mutexChildren);
            if (cur->getClassName() == className) return cur;
        }
        cur = cur->parent.lock();
    }
    return nullptr;
}

std::string Instance::getFullName() {
    InstancePtr self;
    try {
        self = shared_from_this();
    }
    catch (const std::bad_weak_ptr&) {
        return name;
    }
    std::vector<std::string> names;
    InstancePtr current = self;
    while (current) {
        std::lock_guard<std::mutex> lk(current->m_mutexChildren);
        names.push_back(current->name);
        current = current->parent.lock();
    }
    std::string result;
    for (auto it = names.rbegin(); it != names.rend(); ++it) {
        if (!result.empty()) result += '.';
        result += *it;
    }
    return result;
}

std::vector<InstancePtr> Instance::getChildren() const {
    Instance* self = const_cast<Instance*>(this);
    std::lock_guard<std::mutex> lock(self->m_mutexChildren);
    return m_children;
}

void Instance::setParent(const InstancePtr& newParent) {
    if (newParent == nullptr) {
        parent = newParent;
        return;
    }
    InstancePtr oldParent = parent.lock();

    if (oldParent == newParent) {
        return;
    }

    std::vector<std::mutex*> mutexPtrs;
    mutexPtrs.push_back(&m_mutexChildren);
    if (oldParent) mutexPtrs.push_back(&oldParent->m_mutexChildren);
    if (newParent) mutexPtrs.push_back(&newParent->m_mutexChildren);

    std::sort(mutexPtrs.begin(), mutexPtrs.end());
    mutexPtrs.erase(std::unique(mutexPtrs.begin(), mutexPtrs.end()), mutexPtrs.end());

    std::vector<std::unique_lock<std::mutex>> locks;
    locks.reserve(mutexPtrs.size());
    for (auto mptr : mutexPtrs) {
        locks.emplace_back(*mptr);
    }

    if (oldParent) {
        auto& oldChildren = oldParent->m_children;
        auto it = std::find_if(oldChildren.begin(), oldChildren.end(),
            [this](const InstancePtr& p) { return p.get() == this; });
        if (it != oldChildren.end()) {
            oldChildren.erase(it);
            try { oldParent->childRemoved.fire(); }
            catch (...) {}
        }
    }

    if (newParent) {
        auto& newChildren = newParent->m_children;
        bool found = false;
        for (const auto& c : newChildren) {
            if (c.get() == this) { found = true; break; }
        }
        if (!found) {
            newChildren.push_back(shared_from_this());
            try { newParent->childAdded.fire(); }
            catch (...) {}
        }
        parent = newParent;
    }
    else {
        parent.reset();
    }
}

void Instance::destroy() {
    std::vector<InstancePtr> localChildren;
    {
        std::lock_guard<std::mutex> lock(m_mutexChildren);
        localChildren = m_children;
    }

    for (auto& ch : localChildren) {
        if (ch) ch->destroy();
    }

    InstancePtr par = parent.lock();
    if (par) {
        std::mutex* a = &m_mutexChildren;
        std::mutex* b = &par->m_mutexChildren;
        if (a == b) {
            std::lock_guard<std::mutex> lk(*a);
            auto& pcs = par->m_children;
            auto it = std::find_if(pcs.begin(), pcs.end(), [this](const InstancePtr& p) { return p.get() == this; });
            if (it != pcs.end()) pcs.erase(it);
            try { par->childRemoved.fire(); }
            catch (...) {}
        }
        else {
            std::mutex* first = a < b ? a : b;
            std::mutex* second = a < b ? b : a;
            std::lock_guard<std::mutex> l1(*first);
            std::lock_guard<std::mutex> l2(*second);
            auto& pcs = par->m_children;
            auto it = std::find_if(pcs.begin(), pcs.end(), [this](const InstancePtr& p) { return p.get() == this; });
            if (it != pcs.end()) pcs.erase(it);
            try { par->childRemoved.fire(); }
            catch (...) {}
        }
        parent.reset();
    }

    try { destroyed.fire(); }
    catch (...) {}

    {
        std::lock_guard<std::mutex> lock(m_mutexChildren);
        m_children.clear();
    }
}

InstancePtr Instance::clone() const {
    auto inst = std::make_shared<Instance>();
    inst->name = name;
    std::vector<InstancePtr> localChildren;
    {
        Instance* self = const_cast<Instance*>(this);
        std::lock_guard<std::mutex> lock(self->m_mutexChildren);
        localChildren = m_children;
    }

    for (const auto& ch : localChildren) {
        if (ch) {
            InstancePtr childClone = ch->clone();
            if (childClone) {
                childClone->setParent(inst);
            }
        }
    }

    return inst;
}

std::string Instance::getClassName() {
    std::string rawName = typeid(*this).name();

    const std::string classPrefix = "class ";
    const std::string structPrefix = "struct ";

    if (rawName.starts_with(classPrefix)) {
        return rawName.substr(classPrefix.length());
    }

    if (rawName.starts_with(structPrefix)) {
        return rawName.substr(structPrefix.length());
    }

    return std::string(typeid(*this).name());
}


std::vector<InstancePtr> Instance::getDescendants() const {
    std::vector<InstancePtr> descendants;
    Instance* self = const_cast<Instance*>(this);
    std::lock_guard<std::mutex> lock(self->m_mutexChildren);

    for (const auto& child : m_children) {
        if (child) {
            descendants.push_back(child);
            auto childDescendants = child->getDescendants();
            descendants.insert(descendants.end(), childDescendants.begin(), childDescendants.end());
        }
    }
    return descendants;
}