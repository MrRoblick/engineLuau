// GlbDeserializer.cpp
#include <MeshDeserializer/GlbDeserializer.h>
#include <Mesh/Mesh3D.h>
#include <memory>
#include <string>
#include <fstream>
#include <stdexcept>
#include <print>
#include <vector>
#include <nlohmann/json.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <cstdint>
#include <algorithm>

using json = nlohmann::json;

static uint32_t numComponents(const std::string& type) {
	if (type == "SCALAR") return 1;
	if (type == "VEC2")   return 2;
	if (type == "VEC3")   return 3;
	if (type == "VEC4")   return 4;
	if (type == "MAT2")   return 4;
	if (type == "MAT3")   return 9;
	if (type == "MAT4")   return 16;
	throw std::runtime_error("Unknown accessor type: " + type);
}

static size_t componentByteSize(int componentType) {
	switch (componentType) {
	case 5120: case 5121: return 1; // BYTE, UNSIGNED_BYTE
	case 5122: case 5123: return 2; // SHORT, UNSIGNED_SHORT
	case 5125: case 5126: return 4; // UNSIGNED_INT, FLOAT
	default: throw std::runtime_error("Unknown componentType: " + std::to_string(componentType));
	}
}

static float normalizeSigned(int64_t v, int bits) {
	int64_t maxPos = (1LL << (bits - 1)) - 1;
	if (v == -(1LL << (bits - 1))) return -1.0f;
	return static_cast<float>(v) / static_cast<float>(maxPos);
}

static float normalizeUnsigned(uint64_t v, int bits) {
	uint64_t maxv = (1ULL << bits) - 1ULL;
	if (maxv == 0) return 0.0f;
	return static_cast<float>(v) / static_cast<float>(maxv);
}

// Read accessor as array of floats (count * numComponents floats)
static std::vector<float> readAccessorAsFloatArray(const json& doc, const std::vector<uint8_t>& binData, int accessorIndex) {
	const auto& accessor = doc.at("accessors").at(accessorIndex);
	size_t count = accessor.at("count").get<size_t>();
	int componentType = accessor.at("componentType").get<int>();
	std::string type = accessor.at("type").get<std::string>();
	bool normalized = accessor.value("normalized", false);

	size_t comps = numComponents(type);
	size_t compSize = componentByteSize(componentType);
	size_t elementSize = compSize * comps;

	int bufferViewIndex = accessor.value("bufferView", -1);
	if (bufferViewIndex < 0) throw std::runtime_error("Sparse or missing bufferView not supported in this implementation");

	const auto& bufferView = doc.at("bufferViews").at(bufferViewIndex);
	size_t bvByteOffset = bufferView.value("byteOffset", 0);
	size_t bvByteLength = bufferView.at("byteLength").get<size_t>();
	size_t bvStride = bufferView.value("byteStride", 0);

	size_t accessorByteOffset = accessor.value("byteOffset", 0);
	size_t dataStart = bvByteOffset + accessorByteOffset;
	size_t stride = (bvStride != 0) ? bvStride : elementSize;

	// bounds check
	if (count > 0) {
		size_t lastByteNeeded = dataStart + stride * (count - 1) + elementSize;
		if (lastByteNeeded > binData.size()) {
			throw std::runtime_error("Accessor data out of range of BIN buffer");
		}
	}

	std::vector<float> out;
	out.reserve(count * comps);

	for (size_t i = 0; i < count; ++i) {
		size_t base = dataStart + i * stride;
		for (size_t c = 0; c < comps; ++c) {
			size_t off = base + c * compSize;
			switch (componentType) {
			case 5126: { // FLOAT
				float v;
				std::memcpy(&v, &binData[off], sizeof(float));
				out.push_back(v);
				break;
			}
			case 5125: { // UNSIGNED_INT
				uint32_t v;
				std::memcpy(&v, &binData[off], sizeof(uint32_t));
				if (normalized) out.push_back(normalizeUnsigned(v, 32));
				else out.push_back(static_cast<float>(v));
				break;
			}
			case 5123: { // UNSIGNED_SHORT
				uint16_t v;
				std::memcpy(&v, &binData[off], sizeof(uint16_t));
				if (normalized) out.push_back(normalizeUnsigned(v, 16));
				else out.push_back(static_cast<float>(v));
				break;
			}
			case 5122: { // SHORT (signed 16)
				int16_t v;
				std::memcpy(&v, &binData[off], sizeof(int16_t));
				if (normalized) out.push_back(normalizeSigned(v, 16));
				else out.push_back(static_cast<float>(v));
				break;
			}
			case 5121: { // UNSIGNED_BYTE (8)
				uint8_t v = binData[off];
				if (normalized) out.push_back(normalizeUnsigned(v, 8));
				else out.push_back(static_cast<float>(v));
				break;
			}
			case 5120: { // BYTE (signed 8)
				int8_t v;
				std::memcpy(&v, &binData[off], sizeof(int8_t));
				if (normalized) out.push_back(normalizeSigned(v, 8));
				else out.push_back(static_cast<float>(v));
				break;
			}
			default:
				throw std::runtime_error("Unsupported componentType for float conversion: " + std::to_string(componentType));
			}
		}
	}

	return out;
}

// Read indices accessor -> vector<uint32_t>
static std::vector<uint32_t> readIndices(const json& doc, const std::vector<uint8_t>& binData, int accessorIndex) {
	const auto& accessor = doc.at("accessors").at(accessorIndex);
	size_t count = accessor.at("count").get<size_t>();
	int componentType = accessor.at("componentType").get<int>();

	int bufferViewIndex = accessor.value("bufferView", -1);
	if (bufferViewIndex < 0) throw std::runtime_error("Indices accessor with missing bufferView not supported");

	const auto& bufferView = doc.at("bufferViews").at(bufferViewIndex);
	size_t bvByteOffset = bufferView.value("byteOffset", 0);
	size_t accessorByteOffset = accessor.value("byteOffset", 0);
	size_t dataStart = bvByteOffset + accessorByteOffset;

	size_t compSize = componentByteSize(componentType);
	if (dataStart + compSize * count > binData.size()) throw std::runtime_error("Indices out of range");

	std::vector<uint32_t> out;
	out.reserve(count);

	for (size_t i = 0; i < count; ++i) {
		size_t off = dataStart + i * compSize;
		if (componentType == 5121) { // UNSIGNED_BYTE
			uint8_t v = binData[off];
			out.push_back(static_cast<uint32_t>(v));
		}
		else if (componentType == 5123) { // UNSIGNED_SHORT
			uint16_t v;
			std::memcpy(&v, &binData[off], sizeof(uint16_t));
			out.push_back(static_cast<uint32_t>(v));
		}
		else if (componentType == 5125) { // UNSIGNED_INT
			uint32_t v;
			std::memcpy(&v, &binData[off], sizeof(uint32_t));
			out.push_back(v);
		}
		else {
			throw std::runtime_error("Unsupported index componentType: " + std::to_string(componentType));
		}
	}

	return out;
}

// Build local transform matrix for a node
static glm::mat4 nodeLocalMatrix(const json& node) {
	if (node.contains("matrix")) {
		const auto& m = node.at("matrix");
		if (!m.is_array() || m.size() != 16) throw std::runtime_error("Node.matrix must be 16 floats");
		glm::mat4 mat(1.0f);
		for (size_t i = 0; i < 16; ++i) {
			(&mat[0][0])[i] = m.at(i).get<float>();
		}
		return mat;
	}
	else {
		glm::vec3 translation(0.0f);
		glm::quat rotation(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 scale(1.0f);

		if (node.contains("translation")) {
			auto t = node.at("translation");
			translation = glm::vec3(t.at(0).get<float>(), t.at(1).get<float>(), t.at(2).get<float>());
		}
		if (node.contains("rotation")) {
			auto r = node.at("rotation");
			rotation = glm::quat(r.at(3).get<float>(), r.at(0).get<float>(), r.at(1).get<float>(), r.at(2).get<float>());
		}
		if (node.contains("scale")) {
			auto s = node.at("scale");
			scale = glm::vec3(s.at(0).get<float>(), s.at(1).get<float>(), s.at(2).get<float>());
		}

		glm::mat4 m = glm::mat4(1.0f);
		m = glm::translate(m, translation);
		m *= glm::mat4_cast(rotation);
		m = glm::scale(m, scale);
		return m;
	}
}

// Recursively traverse nodes from given root(s) and compute world matrices
static void traverseNodesComputeWorld(const json& doc, int nodeIndex, const glm::mat4& parentMatrix, std::vector<glm::mat4>& outWorldMatrices) {
	const auto& nodes = doc.at("nodes");
	const auto& node = nodes.at(nodeIndex);
	glm::mat4 local = nodeLocalMatrix(node);
	glm::mat4 world = parentMatrix * local;
	outWorldMatrices[nodeIndex] = world;

	if (node.contains("children")) {
		for (const auto& childIdx : node.at("children")) {
			int ci = childIdx.get<int>();
			traverseNodesComputeWorld(doc, ci, world, outWorldMatrices);
		}
	}
}

std::pair<std::vector<Mesh3D::Vertex>, std::vector<uint32_t>> GlbDeserializer::deserialize(const std::string& filename) {
	std::ifstream file{ filename, std::ios::binary };
	std::println("-- glb file deserializer (merge everything into one mesh) --");

	if (!file.is_open()) {
		throw std::runtime_error("Cannot open file: " + filename);
	}

	char magic[5] = { 0 };
	file.read(magic, 4);
	std::println("magic {}", magic);
	if (!file) throw std::runtime_error("Failed to read magic");

	if (std::memcmp(magic, "glTF", 4)) {
		throw std::runtime_error("File is not glb (bad magic)");
	}

	uint32_t version = 0;
	uint32_t totalLength = 0;
	file.read(reinterpret_cast<char*>(&version), sizeof(version));
	file.read(reinterpret_cast<char*>(&totalLength), sizeof(totalLength));
	if (!file) throw std::runtime_error("Failed to read header fields");

	if (version != 2) {
		throw std::runtime_error("Version isn't supported: " + std::to_string(version));
	}

	std::println("version {}", version);
	std::println("total length {}", totalLength);

	json jsonDoc;
	std::vector<uint8_t> binData;
	uint32_t bytesRead = 12;

	while (bytesRead < totalLength && file) {
		uint32_t chunkLength = 0;
		file.read(reinterpret_cast<char*>(&chunkLength), sizeof(chunkLength));
		if (!file) throw std::runtime_error("Failed to read chunkLength");
		char chunkType[5] = { 0 };
		file.read(chunkType, 4);
		if (!file) throw std::runtime_error("Failed to read chunkType");

		bytesRead += 8;
		std::println("chunk type '{}' length {}", std::string(chunkType, 4), chunkLength);

		std::vector<char> chunkData(chunkLength);
		file.read(chunkData.data(), chunkLength);
		if (!file) throw std::runtime_error("Failed to read chunk data");
		bytesRead += chunkLength;

		if (!memcmp(chunkType, "JSON", 4)) {
			std::string s(chunkData.begin(), chunkData.end());
			jsonDoc = json::parse(s);
		}
		else if (!memcmp(chunkType, "BIN", 4) || !memcmp(chunkType, "BIN\0", 4)) {
			binData.assign(reinterpret_cast<uint8_t*>(chunkData.data()), reinterpret_cast<uint8_t*>(chunkData.data()) + chunkLength);
		}
	}

	if (jsonDoc.is_null()) throw std::runtime_error("No JSON chunk found in GLB");

	// prepare node world matrices
	size_t nodeCount = jsonDoc.value("nodes", json::array()).size();
	std::vector<glm::mat4> worldMatrices(nodeCount, glm::mat4(1.0f));

	// determine scene roots
	int sceneIndex = jsonDoc.value("scene", 0);
	if (!jsonDoc.contains("scenes")) throw std::runtime_error("No scenes in glTF");
	const auto& scenes = jsonDoc.at("scenes");
	if (sceneIndex < 0 || sceneIndex >= (int)scenes.size()) sceneIndex = 0;
	const auto& scene = scenes.at(sceneIndex);

	if (scene.contains("nodes")) {
		for (const auto& rootNode : scene.at("nodes")) {
			int rootIdx = rootNode.get<int>();
			traverseNodesComputeWorld(jsonDoc, rootIdx, glm::mat4(1.0f), worldMatrices);
		}
	}
	else {
		// if no scene nodes, but nodes exist, consider all nodes as potential roots
		for (size_t i = 0; i < nodeCount; ++i) {
			traverseNodesComputeWorld(jsonDoc, static_cast<int>(i), glm::mat4(1.0f), worldMatrices);
		}
	}

	std::vector<Mesh3D::Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve(1024);
	indices.reserve(1024);

	// For each node that references a mesh, read its meshes/primitives
	if (jsonDoc.contains("nodes")) {
		const auto& nodes = jsonDoc.at("nodes");
		for (size_t ni = 0; ni < nodes.size(); ++ni) {
			const auto& node = nodes.at(ni);
			if (!node.contains("mesh")) continue;
			int meshIndex = node.at("mesh").get<int>();
			if (!jsonDoc.contains("meshes")) continue;
			const auto& meshes = jsonDoc.at("meshes");
			if (meshIndex < 0 || meshIndex >= (int)meshes.size()) continue;
			const auto& mesh = meshes.at(meshIndex);

			if (!mesh.contains("primitives")) continue;
			const auto& primitives = mesh.at("primitives");

			for (const auto& prim : primitives) {
				// attributes
				if (!prim.contains("attributes")) continue;
				const auto& attrs = prim.at("attributes");

				if (!attrs.contains("POSITION")) throw std::runtime_error("POSITION attribute required");

				int posAccessor = attrs.at("POSITION").get<int>();
				int normAccessor = attrs.value("NORMAL", -1);
				int texAccessor = attrs.value("TEXCOORD_0", -1);
				int colorAccessor = attrs.value("COLOR_0", -1);

				auto positions = readAccessorAsFloatArray(jsonDoc, binData, posAccessor); // count * 3
				std::vector<float> normals;
				if (normAccessor >= 0) normals = readAccessorAsFloatArray(jsonDoc, binData, normAccessor);
				std::vector<float> texcoords;
				if (texAccessor >= 0) texcoords = readAccessorAsFloatArray(jsonDoc, binData, texAccessor);
				std::vector<float> colors;
				if (colorAccessor >= 0) colors = readAccessorAsFloatArray(jsonDoc, binData, colorAccessor);

				std::vector<uint32_t> primIndices;
				if (prim.contains("indices")) {
					int idxAccessor = prim.at("indices").get<int>();
					primIndices = readIndices(jsonDoc, binData, idxAccessor);
				}
				else {
					// create sequential indices
					size_t pc = positions.size() / 3;
					primIndices.resize(pc);
					for (size_t i = 0; i < pc; ++i) primIndices[i] = static_cast<uint32_t>(i);
				}

				// apply world matrix for this node
				glm::mat4 world = worldMatrices[ni];
				glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(world)));

				size_t vertexOffset = vertices.size();
				size_t vertexCount = positions.size() / 3;

				// append vertices (transform positions, normals, colors, texcoords)
				for (size_t v = 0; v < vertexCount; ++v) {
					Mesh3D::Vertex mv;
					glm::vec4 pos4(positions[v * 3 + 0], positions[v * 3 + 1], positions[v * 3 + 2], 1.0f);
					glm::vec4 wp = world * pos4;
					mv.position = glm::vec3(wp.x, wp.y, wp.z);

					if (!normals.empty()) {
						size_t ncount = normals.size() / 3;
						if (v < ncount) {
							glm::vec3 n(normals[v * 3 + 0], normals[v * 3 + 1], normals[v * 3 + 2]);
							glm::vec3 tn = normalMat * n;
							mv.normal = glm::normalize(tn);
						}
						else {
							mv.normal = glm::vec3(0.0f, 0.0f, 1.0f);
						}
					}
					else {
						mv.normal = glm::vec3(0.0f, 0.0f, 1.0f);
					}

					// vertex color: support VEC3 or VEC4 (ignore alpha), normalized handled by reader
					if (!colors.empty()) {
						size_t colorComps = 0;
						// determine components per element by dividing length by vertexCount (guard)
						if (vertexCount > 0) colorComps = colors.size() / vertexCount;
						if (colorComps >= 3) {
							mv.vertexColor = glm::vec3(colors[v * colorComps + 0],
								colors[v * colorComps + 1],
								colors[v * colorComps + 2]);
						}
						else if (colorComps == 1) {
							// single-channel color -> replicate
							float c = colors[v];
							mv.vertexColor = glm::vec3(c, c, c);
						}
						else {
							mv.vertexColor = glm::vec3(1.0f);
						}
					}
					else {
						mv.vertexColor = glm::vec3(1.0f); // default white
					}

					if (!texcoords.empty()) {
						size_t tcount = texcoords.size() / 2;
						if (v < tcount) {
							mv.textureCoords = glm::vec2(texcoords[v * 2 + 0], texcoords[v * 2 + 1]);
						}
						else {
							mv.textureCoords = glm::vec2(0.0f, 0.0f);
						}
					}
					else {
						mv.textureCoords = glm::vec2(0.0f, 0.0f);
					}

					vertices.push_back(mv);
				}

				// append indices with offset
				for (uint32_t idx : primIndices) {
					indices.push_back(static_cast<uint32_t>(vertexOffset) + idx);
				}
			} // end primitives
		} // end nodes loop
	}

	// If no vertices collected, still check top-level meshes (some files reference meshes only from scenes, others via nodes)
	if (vertices.empty() && jsonDoc.contains("meshes")) {
		const auto& meshes = jsonDoc.at("meshes");
		for (size_t meshIndex = 0; meshIndex < meshes.size(); ++meshIndex) {
			const auto& mesh = meshes.at(meshIndex);
			if (!mesh.contains("primitives")) continue;
			const auto& primitives = mesh.at("primitives");
			glm::mat4 world = glm::mat4(1.0f); // default
			for (const auto& prim : primitives) {
				if (!prim.contains("attributes")) continue;
				const auto& attrs = prim.at("attributes");
				if (!attrs.contains("POSITION")) throw std::runtime_error("POSITION attribute required");

				int posAccessor = attrs.at("POSITION").get<int>();
				int normAccessor = attrs.value("NORMAL", -1);
				int texAccessor = attrs.value("TEXCOORD_0", -1);
				int colorAccessor = attrs.value("COLOR_0", -1);

				auto positions = readAccessorAsFloatArray(jsonDoc, binData, posAccessor); // count * 3
				std::vector<float> normals;
				if (normAccessor >= 0) normals = readAccessorAsFloatArray(jsonDoc, binData, normAccessor);
				std::vector<float> texcoords;
				if (texAccessor >= 0) texcoords = readAccessorAsFloatArray(jsonDoc, binData, texAccessor);
				std::vector<float> colors;
				if (colorAccessor >= 0) colors = readAccessorAsFloatArray(jsonDoc, binData, colorAccessor);

				std::vector<uint32_t> primIndices;
				if (prim.contains("indices")) {
					int idxAccessor = prim.at("indices").get<int>();
					primIndices = readIndices(jsonDoc, binData, idxAccessor);
				}
				else {
					// create sequential indices
					size_t pc = positions.size() / 3;
					primIndices.resize(pc);
					for (size_t i = 0; i < pc; ++i) primIndices[i] = static_cast<uint32_t>(i);
				}

				glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(world)));
				size_t vertexOffset = vertices.size();
				size_t vertexCount = positions.size() / 3;

				for (size_t v = 0; v < vertexCount; ++v) {
					Mesh3D::Vertex mv;
					glm::vec4 pos4(positions[v * 3 + 0], positions[v * 3 + 1], positions[v * 3 + 2], 1.0f);
					glm::vec4 wp = world * pos4;
					mv.position = glm::vec3(wp.x, wp.y, wp.z);

					if (!normals.empty()) {
						size_t ncount = normals.size() / 3;
						if (v < ncount) {
							glm::vec3 n(normals[v * 3 + 0], normals[v * 3 + 1], normals[v * 3 + 2]);
							glm::vec3 tn = normalMat * n;
							mv.normal = glm::normalize(tn);
						}
						else {
							mv.normal = glm::vec3(0.0f, 0.0f, 1.0f);
						}
					}
					else {
						mv.normal = glm::vec3(0.0f, 0.0f, 1.0f);
					}

					// vertex color
					if (!colors.empty()) {
						size_t colorComps = 0;
						if (vertexCount > 0) colorComps = colors.size() / vertexCount;
						if (colorComps >= 3) {
							mv.vertexColor = glm::vec3(colors[v * colorComps + 0],
								colors[v * colorComps + 1],
								colors[v * colorComps + 2]);
						}
						else if (colorComps == 1) {
							float c = colors[v];
							mv.vertexColor = glm::vec3(c, c, c);
						}
						else {
							mv.vertexColor = glm::vec3(1.0f);
						}
					}
					else {
						mv.vertexColor = glm::vec3(1.0f);
					}

					if (!texcoords.empty()) {
						size_t tcount = texcoords.size() / 2;
						if (v < tcount) {
							mv.textureCoords = glm::vec2(texcoords[v * 2 + 0], texcoords[v * 2 + 1]);
						}
						else {
							mv.textureCoords = glm::vec2(0.0f, 0.0f);
						}
					}
					else {
						mv.textureCoords = glm::vec2(0.0f, 0.0f);
					}

					vertices.push_back(mv);
				}

				for (uint32_t idx : primIndices) {
					indices.push_back(static_cast<uint32_t>(vertexOffset) + idx);
				}
			}
		}
	}

	return { std::move(vertices), std::move(indices) };
}
