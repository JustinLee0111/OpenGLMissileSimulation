#include <vector>

#include "missile_hud.h"
#include "missile/missile.h"

void MissileHud::MissileHUD(std::vector<Missile*>& missiles)
{
    static int location = 1;
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
    if (location >= 0)
    {
        const float PAD = 20.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImVec2 window_pos, window_pos_pivot;
        window_pos.x = (work_pos.x + work_size.x - PAD);
        window_pos.y = (work_pos.y + PAD);
        window_pos_pivot.x = 1.0f;
        window_pos_pivot.y = 0.0f;
        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }

    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

    if (ImGui::Begin("Missile HUD", nullptr, window_flags))
    {
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Missile HUD");
        ImGui::Separator();
        if (!missiles.empty()) {
            ImGui::Text("Speed: %.1f m/s", glm::length(missiles[0]->physicsProperties->velocity));
            ImGui::Text("GForce: %.1f G", (glm::length(missiles[0]->physicsProperties->angularVelocity) * glm::length(missiles[0]->physicsProperties->velocity)) / 9.81f);
            ImGui::Text("Angle of Attack (deg): %.1f", glm::degrees(missiles[0]->getAOA()));
            ImGui::Text("Fuel Remaining (sec): %.1f", missiles[0]->getFuelTimeLeft());
            if (missiles[0]->engineOn) {
                ImGui::Text("Engine: On");
            }
            else {
                ImGui::Text("Engine: Off");
            }
            if (missiles[0]->getSeeker().getIsTracking()) {
                ImGui::Text("Seeker: Tracking");
            }
            else {
                ImGui::Text("Seeker: Not Tracking");
            }
            if (ImGui::Button("Reset Fuel")) {
                missiles[0]->resetFuel();
            }
        }
        else {
            ImGui::Text("No Missile!");
        }  
    }
    ImGui::End();
}