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
    std::lock_guard<std::mutex> lock(mutexChildren);
    for (const auto& c : children) {
        if (c && c->name == name) {
            return c;
        }
    }
    return nullptr;
}

InstancePtr Instance::findFirstChildOfClass(const std::string& className) {
    std::lock_guard<std::mutex> lock(mutexChildren);
    for (const auto& c : children) {
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
            std::lock_guard<std::mutex> lk(cur->mutexChildren);
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
            std::lock_guard<std::mutex> lk(cur->mutexChildren);
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
        std::lock_guard<std::mutex> lk(mutexChildren);
        return name;
    }

    std::vector<InstancePtr> chain;
    chain.reserve(8);
    chain.push_back(self);

    while (true) {
        InstancePtr cur = chain.back();
        InstancePtr par;
        {
            std::lock_guard<std::mutex> lock(cur->mutexChildren);
            par = cur->parent.lock();
        }
        if (!par) break;
        chain.push_back(par);
    }
    std::string result;
    bool first = true;
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        InstancePtr node = *it;
        std::lock_guard<std::mutex> lk(node->mutexChildren);
        if (!first) result += '.';
        result += node->name;
        first = false;
    }

    return result;
}

std::vector<InstancePtr> Instance::getChildren() const {
    Instance* self = const_cast<Instance*>(this);
    std::lock_guard<std::mutex> lock(self->mutexChildren);
    return children;
}

void Instance::setParent(const InstancePtr& newParent) {
    InstancePtr oldParent = parent.lock();

    if (oldParent == newParent) {
        return;
    }

    std::vector<std::mutex*> mutexPtrs;
    mutexPtrs.push_back(&mutexChildren);
    if (oldParent) mutexPtrs.push_back(&oldParent->mutexChildren);
    if (newParent) mutexPtrs.push_back(&newParent->mutexChildren);

    std::sort(mutexPtrs.begin(), mutexPtrs.end());
    mutexPtrs.erase(std::unique(mutexPtrs.begin(), mutexPtrs.end()), mutexPtrs.end());

    std::vector<std::unique_lock<std::mutex>> locks;
    locks.reserve(mutexPtrs.size());
    for (auto mptr : mutexPtrs) {
        locks.emplace_back(*mptr);
    }

    if (oldParent) {
        auto& oldChildren = oldParent->children;
        auto it = std::find_if(oldChildren.begin(), oldChildren.end(),
            [this](const InstancePtr& p) { return p.get() == this; });
        if (it != oldChildren.end()) {
            oldChildren.erase(it);
            try { oldParent->childRemoved.fire(); }
            catch (...) {}
        }
    }

    if (newParent) {
        auto& newChildren = newParent->children;
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
        std::lock_guard<std::mutex> lock(mutexChildren);
        localChildren = children;
    }

    for (auto& ch : localChildren) {
        if (ch) ch->destroy();
    }

    InstancePtr par = parent.lock();
    if (par) {
        std::mutex* a = &mutexChildren;
        std::mutex* b = &par->mutexChildren;
        if (a == b) {
            std::lock_guard<std::mutex> lk(*a);
            auto& pcs = par->children;
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
            auto& pcs = par->children;
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
        std::lock_guard<std::mutex> lock(mutexChildren);
        children.clear();
    }
}

InstancePtr Instance::clone() const {
    auto inst = std::make_shared<Instance>();
    inst->name = name;
    inst->m_className = m_className;

    std::vector<InstancePtr> localChildren;
    {
        Instance* self = const_cast<Instance*>(this);
        std::lock_guard<std::mutex> lock(self->mutexChildren);
        localChildren = children;
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
    if (!m_className.empty()) return m_className;
    return std::string(typeid(*this).name());
}
