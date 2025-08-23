#include <Instance/DataModel.h>
#include <memory>

std::shared_ptr<DataModel> DataModel::getInstance(){
	static auto instance = std::make_shared<DataModel>();
	instance->name = "DataModel";
	return instance;
}