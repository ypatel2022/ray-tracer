#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Camera.h"
#include "Renderer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer {
public:
    ExampleLayer()
        : m_Camera(45.0f, 0.1f, 100.0f)
    {
        {
            // sun
            Material& orangeSphere = m_Scene.Materials.emplace_back();
            orangeSphere.Albedo = { 1.0f, 1.0f, 1.0f };
            orangeSphere.Roughness = 0.1f;
            orangeSphere.EmissionColor = orangeSphere.Albedo;
            orangeSphere.EmissionPower = 100.0f;

            Sphere sphere;
            sphere.Position = { 10.0f, 20.0f, 0.0f };
            sphere.Radius = 1.0f;
            sphere.MaterialIndex = 0;
            m_Scene.Spheres.push_back(sphere);
        }
        {
            // floor
            Material& material = m_Scene.Materials.emplace_back();
            material.Albedo = { 0.7f, 0.7f, 0.7f };
            material.Roughness = 0.1f;

            Sphere sphere;
            sphere.Position = { 0.0f, -251.0f, 0.0f };
            sphere.Radius = 250.0f;
            sphere.MaterialIndex = 1;
            m_Scene.Spheres.push_back(sphere);
        }

        // create some random materials and spheres
        for (int i = 0; i < 25; i++) {
            Material& material = m_Scene.Materials.emplace_back();
            material.Albedo = Walnut::Random::Vec3(0.0f, 1.0f);
            material.Roughness = Walnut::Random::Float();

            material.EmissionColor = material.Albedo;
            // material.EmissionPower = Walnut::Random::Float() * 2.5f;
            material.EmissionPower = 0.f;

            Sphere sphere;

            float x = RandomFloat(3.0f, -3.0f);
            float y = RandomFloat(-0.5f, -0.7f);
            float z = RandomFloat(3.0f, -3.0f);

            sphere.Position = glm::vec3(x, y, z);
            sphere.Radius = RandomFloat(0.1f, 0.2f);
            sphere.MaterialIndex = i + 2;
            m_Scene.Spheres.push_back(sphere);
        }
    }

    virtual void OnUpdate(float ts) override
    {
        if (m_Camera.OnUpdate(ts)) {
            m_Renderer.ResetFrameIndex();
        }
    }

    virtual void OnUIRender() override
    {
        // settings window
        ImGui::Begin("Settings");

        ImGui::Text("Last render %.3fms", m_LastRenderTIme);

        if (ImGui::Button("Render")) {
            Render();
        }

        ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

        ImGui::Checkbox("Slow Random", &m_Renderer.GetSettings().SlowRandom);

        ImGui::DragInt("Bounces", &m_Renderer.GetSettings().Bounces, 0, 1, 10);

        if (ImGui::Button("Reset")) {
            m_Renderer.ResetFrameIndex();
        }

        ImGui::End();

        // scene window
        ImGui::Begin("Scene");
        for (size_t i = 0; i < m_Scene.Spheres.size(); i++) {
            ImGui::PushID(i);

            Sphere& sphere = m_Scene.Spheres[i];
            ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.Radius, 0.1f, 0.0f, 250.0f);
            ImGui::DragInt("Material", &sphere.MaterialIndex, 1, 0, (int)m_Scene.Materials.size() - 1);

            ImGui::Separator();

            ImGui::PopID();
        }

        for (size_t i = 0; i < m_Scene.Materials.size(); i++) {
            ImGui::PushID(i);

            Material& mat = m_Scene.Materials[i];

            ImGui::ColorEdit3("Albedo", glm::value_ptr(mat.Albedo));
            ImGui::DragFloat("Roughness", &mat.Roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &mat.Metallic, 0.05f, 0.0f, 1.0f);
            ImGui::ColorEdit3("Emission Color", glm::value_ptr(mat.EmissionColor));
            ImGui::DragFloat("Emission Power", &mat.EmissionPower, 0.05f, 0.0f, FLT_MAX);

            ImGui::PopID();
        }

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        m_ViewportWidth = ImGui::GetContentRegionAvail().x;
        m_ViewportHeight = ImGui::GetContentRegionAvail().y;

        auto image = m_Renderer.GetFinalImage();

        if (image) {
            ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGui::End();
        ImGui::PopStyleVar();

        Render();
    }

    void Render()
    {
        Timer timer;

        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render(m_Scene, m_Camera);

        m_LastRenderTIme = timer.ElapsedMillis();
    }

    float RandomFloat(float a, float b)
    {
        float random = Walnut::Random::Float();
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }

private:
    Renderer m_Renderer;
    Camera m_Camera;
    Scene m_Scene;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    float m_LastRenderTIme = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
    Walnut::ApplicationSpecification spec;
    spec.Name = "Walnut Example";

    Walnut::Application* app = new Walnut::Application(spec);
    app->PushLayer<ExampleLayer>();
    app->SetMenubarCallback([app]() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}