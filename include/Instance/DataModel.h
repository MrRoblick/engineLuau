#pragma once
#include <Instance/Instance.h>
#include <memory>


class DataModel : public Instance  {
public:
	static std::shared_ptr<DataModel> getInstance();

	//void destroy() override = 0;
	//InstancePtr clone() const = 0;

	~DataModel() = default;
	DataModel() = default;
private:
	DataModel(const DataModel&) = delete;
	DataModel& operator=(const DataModel&) = delete;
};