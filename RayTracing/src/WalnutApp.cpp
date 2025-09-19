#define WL_PLATFORM_WINDOWS
#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Utilities.h"
#include "constant_medium.h"

#include "Renderer.h"
#include "bvh.h"
#include "Camera.h"
#include "sphere.h"
#include "aarect.h"
#include "box.h"

#include "material.h"
#include <glm/gtc/type_ptr.hpp>
using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	hittable_list Random() {

		hittable_list world;
		auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
		world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

		for (int a = -11; a < 11; a++) {
			for (int b = -11; b < 11; b++) {
				auto choose_mat = Rand::RandomFloat();
				point3 center(a + 0.9 * Rand::RandomFloat(), 0.2, b + 0.9 * Rand::RandomFloat());

				if ((center - point3(4, 0.2, 0)).length() > 0.9) {
					shared_ptr<material> sphere_material;
					if (choose_mat < 0.3){
						auto albedo = Rand::random() * Rand::random();
						sphere_material = make_shared<diffuse_light>(400.f*albedo);
						world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
					}
					else if (choose_mat < 0.8) {
						// diffuse
						auto albedo = Rand::random() * Rand::random();
						sphere_material = std::make_shared<lambertian>(albedo);
						world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
					}
					else if (choose_mat < 0.95) {
						// metal
						auto albedo = Rand::random(0.5, 1);
						auto fuzz = Rand::random_double(0, 0.5);
						sphere_material = make_shared<metal>(albedo, fuzz);
						world.add(make_shared<sphere>(center, 0.2, sphere_material));
					}
					else {
						// glass
						sphere_material = make_shared<dielectric>(1.5);
						world.add(make_shared<sphere>(center, 0.2, sphere_material));
					}
				}
			}
		}

		auto material1 = make_shared<dielectric>(1.5);
		world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	//	auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
	//	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, emat));

		auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
		world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
		return world;

	}
	hittable_list random_scene() {
		hittable_list boxes1;
		auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));
		hittable_list objects;

		const int boxes_per_side = 20;
		for (int i = 0; i < boxes_per_side; i++) {
			for (int j = 0; j < boxes_per_side; j++) {
				auto w = 100.0;
				auto x0 = -1000.0 + i * w;
				auto z0 = -1000.0 + j * w;
				auto y0 = 0.0;
				auto x1 = x0 + w;
				auto y1 = Rand::random_double(1, 101);
				auto z1 = z0 + w;

				boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
			}
		}


		objects.add(make_shared<bvh_node>(boxes1));
		
		auto light = make_shared<diffuse_light>(color(60, 60, 60));
		objects.add(make_shared<xz_rect>(50, 323, -47, 312, 554, light));

		auto center1 = point3(400, 400, 200);
		auto moving_sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
		objects.add(make_shared<sphere>(center1,  50, moving_sphere_material));

		objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
		objects.add(make_shared<sphere>(
			point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
			));

		auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
		objects.add(boundary);
		objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
		boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
		objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

		auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
		objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
		auto pertext = make_shared<noise_texture>(0.1);
		objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

		hittable_list boxes2;
		auto white = make_shared<lambertian>(color(1, 1, 1));
		int ns = 1;
		for (int j = 0; j < ns; j++) {
			boxes2.add(make_shared<sphere>(Rand::random(0, 165), 10, white));
		}

		objects.add(make_shared<translate>(
				make_shared<bvh_node>(boxes2),
			glm::vec3(-100, 270, 395)
			)
		);
		
		return objects;
	
	}
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{

		m_World = Random();
		m_WorldRen = &m_World;
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}

	virtual void OnUIRender() override
	{
		SettingsUI();
		 ViewportUI();
		 Render(m_RenderSettings);

	}
	//Codul folosit pentru randarea imagini pe ecran
	void ViewportUI()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");
		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();

		if (image)

		ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
			ImVec2(0, 1), ImVec2(1, 0));
	
		ImGui::End();
		ImGui::PopStyleVar();
	}


	void SettingsUI()
	{
		
		ImGui::Begin("Settings");

		Renderer::Settings& settings = m_RenderSettings;
		ImGui::ColorEdit3("Background", glm::value_ptr(settings.Background));
		ImGui::InputInt("RendPerPixel", &settings.samples_per_pixel,1, 0, 0);
		ImGui::InputInt("Bounces", &settings.bounces,1, 0, 0);
		ImGui::InputFloat("GammaCor", &settings.gammaCor,1, 0, 0);
		ImGui::Checkbox("Par pe Pix", &settings.ParalelizePerPixel);
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		ImGui::Text("Framerate: %.1f", 1000 / m_LastRenderTime);
		ImGui::End();


	}
	
	void Render(Renderer::Settings& settings)
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Camera, bvh_node(m_World), settings);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer::Settings m_RenderSettings;
	Renderer::Settings m_FRenderSettings;
	Renderer m_Renderer;
	Camera m_Camera;
	hittable_list m_World;
	hittable* m_WorldRen;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
		
}