#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <limits>
#include <memory>
using std::shared_ptr;
namespace Rand {
	 thread_local static uint32_t seed;
	 inline uint32_t PSG_Hash(uint32_t input)
	 {
		 uint32_t state = input * 747796405u + 2891336453u;
		 uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		 return (word >> 22u) ^ word;
	 }
	inline float RandomFloat() {
		seed = PSG_Hash(seed);
		return (float)seed / (float)std::numeric_limits<uint32_t>::max();
	}
	
	inline float random_double(float min, float max) {
		// Returns a random real in [min,max).
		return min + (max - min) * RandomFloat();
	}
	inline int random_int(int min, int max) {
		// Returns a random integer in [min,max].
		return static_cast<int>(random_double(min, max + 1));
	}
	inline static glm::vec3 random() {
		return glm::vec3(RandomFloat(), RandomFloat(), RandomFloat());
	}

	inline static glm::vec3 random(float min, float max) {
		return glm::vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}
	inline static glm::vec3 random_in_unit_sphere() {
		while (true) {
			auto p = Rand::random(-1, 1);
			if (dot(p,p) >= 1) continue;
			return p;
		}

	}
	inline static glm::vec3 random_unit_vector() {
		return glm::normalize(random_in_unit_sphere());
	}
	inline static glm::vec3 random_in_hemisphere(const glm::vec3& normal) {
		glm::vec3 in_unit_sphere = random_in_unit_sphere();
		if (glm::dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
			return in_unit_sphere;
		else
			return -in_unit_sphere;
	}
	
}
typedef glm::vec3 color;
typedef glm::vec3 point3;
namespace Utils {
	// Usings
	using std::shared_ptr;
	using std::make_shared;
	using std::sqrt;
	using glm::vec3;

	// Constants

	const double infinity = std::numeric_limits<double>::infinity();
	const double pi = 3.1415926535897932385;

	// Utility Functions

	inline double degrees_to_radians(double degrees) {
		return degrees * pi / 180.0;
	}
	inline float lerp(float a, float b, float t) {
		return a * t + b * (1 - t);
	}
	// Common Headers
	inline float clamp(float value, float min, float max) {
		if (value < min)
			return min;
		if (value > max)
			return max;
		return value;
	}
	inline static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		
		uint8_t r = (uint8_t)(clamp(color.r,0.f,1.f) * 255.0f);
		uint8_t g = (uint8_t)(clamp(color.g, 0.f, 1.f) * 255.0f);
		uint8_t b = (uint8_t)(clamp(color.b, 0.f, 1.f) * 255.0f);
		uint8_t a = (uint8_t)(clamp(color.a, 0.f, 1.f) * 255.0f);

		uint32_t result = (a << 24) | (b << 16) | (g << 8) | r;
		return result;
	}
	inline static glm::vec4 ConvertToVec4(const uint32_t color) {
		float r = ((float)(color         % 256)) / 255.0f;
		float g = ((float)(color/(1<<8 ) % 256)) / 255.0f;
		float b = ((float)(color/(1<<16) % 256)) / 255.0f;
		float a = ((float)(color/(1<<24) % 256)) / 255.0f;
		return glm::vec4(r,g,b,a);
	}
	inline static glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t) {
		return a * (1 - t) + b * t;
	}
	
	bool near_zero(glm::vec3 vec) {
		// Return true if the vector is close to zero in all dimensions.
		const auto s = 1e-8f;
		return (fabs(vec.x) < s) && (fabs(vec.y) < s) && (fabs(vec.z) < s);
	}
	inline vec3 reflect(const vec3& v, const vec3& n) {
		return v - 2 * dot(v, n) * n;
	}
	inline vec3 refract(const vec3& uv, const vec3& n, float etai_over_etat) {
		float cos_theta = fmin(dot(-uv, n), 1.0);
		vec3 r_out_perp = etai_over_etat * (uv +  cos_theta*n );
		vec3 r_out_parallel =((float)- sqrt(fabs(1.0 - dot(r_out_perp, r_out_perp)))) * n;
		return r_out_perp + r_out_parallel;
	}
	
}
#include "ray.h"
