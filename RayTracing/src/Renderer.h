#pragma once

#include "Walnut/Image.h"

#include "Camera.h"
#include "Utilities.h"
#include <glm/glm.hpp>
#include "hittable_list.h"


class Renderer
{
public:
	struct Settings
	{
		bool ParalelizePerPixel = true;
		float gammaCor=2.2;
		int bounces = 10;
		color Background = color(0.5f,0.7f,0.9f);
		bool Accumulate = true;
		int samples_per_pixel = 1;
	};
public:
	Renderer() = default;
	glm::vec3 lightDir = glm::vec3(-1, -1, -1);
	void OnResize(uint32_t width, uint32_t height);
	void Render(const Camera& camera, const hittable& world,const Settings& settings);

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }

	void ResetFrameIndex() { m_FrameIndex = 1; }
	Settings& GetSettings() { return m_Settings; }
	int GetFrameIndex() { return m_FrameIndex; }
private:

	std::shared_ptr<Walnut::Image> m_FinalImage;
	Settings m_Settings;

	std::vector<uint32_t> m_ImageHorizontalIter, m_ImageVerticalIter;

	const Camera* m_ActiveCamera = nullptr;

	uint32_t* m_ImageData = nullptr;
	glm::vec4* m_AccumulationData = nullptr;
	uint32_t m_FrameIndex = 1;
};