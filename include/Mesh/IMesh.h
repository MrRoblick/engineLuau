#pragma once
#include <glad/glad.h>

class IMesh {
public:
	virtual ~IMesh() = default;
	virtual GLuint getVAO() const = 0;
	virtual GLsizei getIndexCount() const = 0;
};