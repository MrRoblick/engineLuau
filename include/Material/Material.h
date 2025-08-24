#pragma once

class Material {
private:
	
public:
	Material() = default;
	virtual ~Material() = default;

	void apply() const;
};