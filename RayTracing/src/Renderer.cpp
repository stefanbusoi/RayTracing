#include "Renderer.h"
#include "Utilities.h"

#include <vector>
#include "Walnut/Random.h"
#include <execution>
#include <set>

#include "hittable_list.h"
#include "material.h"

#include "ray.h"
using glm::vec3;
void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);
	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
	delete[] m_AccumulationData;
	m_AccumulationData = new glm::vec4[width * height];

	m_ImageHorizontalIter.resize(width);
	m_ImageVerticalIter.resize(height);
	for (uint32_t i = 0; i < width; i++)
		m_ImageHorizontalIter[i] = i;
	for (uint32_t i = 0; i < height; i++)
		m_ImageVerticalIter[i] = i;
	m_FrameIndex = m_Settings.samples_per_pixel;
}


vec3 ray_color(const ray& r, const color& background, const hittable& world,int depth) {
	hit_record rec;
	if (depth <= 0)
		return vec3(0, 0, 0);
	// If the ray hits nothing, return the background color.
	if (!world.hit(r, 0.001, Utils::infinity, rec))
		return background;

	ray scattered;
	color attenuation;
	color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		return emitted;

	return emitted + attenuation * ray_color(scattered, background, world, depth - 1);

}

void Renderer::Render(const Camera& camera,const hittable& world,const Settings& settings) {

	
	auto xDist = camera.GetRayDirections()[1] - camera.GetRayDirections()[0];
	auto yDist = camera.GetRayDirections()[m_FinalImage->GetWidth()] - camera.GetRayDirections()[0];
	if (m_FrameIndex == 1)
		memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));
	m_FinalImage->SetData(m_ImageData);
	if (settings.ParalelizePerPixel == true) {
		std::for_each(std::execution::par, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
			[&world, yDist, xDist, &camera, this, settings](uint32_t y)
			{
				std::for_each(std::execution::par, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
					[&world, yDist, xDist, &camera, this, y, settings](uint32_t x) {
						for (int i = 1; i <= settings.samples_per_pixel; i++) {
							color pixelData(0);

							vec3 RayDid =glm::normalize( camera.GetRayDirections()[x + m_FinalImage->GetWidth() * y]
								+ Rand::random_double(-0.5, 0.5) * xDist
								+ Rand::random_double(-0.5, 0.5) * yDist);
							ray r(camera.GetPosition(), RayDid);
							pixelData = glm::vec3(ray_color(r, settings.Background, world, settings.bounces));
							pixelData.r = pow(pixelData.r, 1 / settings.gammaCor);
							pixelData.g = pow(pixelData.g, 1 / settings.gammaCor);
							pixelData.b = pow(pixelData.b, 1 / settings.gammaCor);
							m_AccumulationData[x + y * m_FinalImage->GetWidth()] += glm::vec4(pixelData, 1);
						}
						m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(m_AccumulationData[x + y * m_FinalImage->GetWidth()] / (float)m_FrameIndex);
					});
			});
	}else {
		std::for_each(std::execution::seq, m_ImageVerticalIter.begin(), m_ImageVerticalIter.end(),
		[&world, yDist, xDist, &camera, this, settings](uint32_t y)
		{
			std::for_each(std::execution::seq, m_ImageHorizontalIter.begin(), m_ImageHorizontalIter.end(),
				[&world, yDist, xDist, &camera, this, y, settings](uint32_t x) {
					for (int i = 1; i <= settings.samples_per_pixel; i++) {
						color pixelData(0);

						vec3 RayDid =glm::normalize( camera.GetRayDirections()[x + m_FinalImage->GetWidth() * y]
							+ Rand::random_double(-0.5, 0.5) * xDist
							+ Rand::random_double(-0.5, 0.5) * yDist);
						ray r(camera.GetPosition(), RayDid);
						pixelData = glm::vec3(ray_color(r, settings.Background, world, settings.bounces));
						pixelData.r = pow(pixelData.r, 1 / settings.gammaCor);
						pixelData.g = pow(pixelData.g, 1 / settings.gammaCor);
						pixelData.b = pow(pixelData.b, 1 / settings.gammaCor);
						m_AccumulationData[x + y * m_FinalImage->GetWidth()] += glm::vec4(pixelData, 1);
					}
					m_ImageData[x + y * m_FinalImage->GetWidth()] = Utils::ConvertToRGBA(m_AccumulationData[x + y * m_FinalImage->GetWidth()] / (float)m_FrameIndex);
				});
		});
	}
	m_FrameIndex += settings.samples_per_pixel;


	
}
