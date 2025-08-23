#pragma once
#include <string>
#include <memory>
#include <vector>
#include <Event/Event.h>
#include <optional>

class Instance;
using InstancePtr = std::shared_ptr<Instance>;
using WeakInstancePtr = std::weak_ptr<Instance>;

class Instance: std::enable_shared_from_this<Instance> {
public:
	Instance() = default;
	virtual ~Instance() {};

	std::string name = "Instance";
	WeakInstancePtr parent;

	Event<> childAdded;
	Event<> childRemoved;
	Event<> destroyed;

	InstancePtr findFirstChild(const std::string& name);
	InstancePtr findFirstChildOfClass(const std::string& className);

	InstancePtr findFirstAncestor(const std::string& name);
	InstancePtr findFirstAncestorOfClass(const std::string& name);

	std::string getFullName();

	std::vector<InstancePtr> getChildren() const;
	void setParent(const InstancePtr& newParent);

	virtual void destroy();
	virtual InstancePtr clone() const;

	std::string getClassName();
private:
	std::string m_className = "Instance";

	std::vector<InstancePtr> children;
	std::mutex mutexChildren;
};