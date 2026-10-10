#define GLFW_INCLUDE_NONE

#include "imgui.h"
#include "app_hud.h"
#include "core/Application.h"
#include "core/LevelManager.h"

void AppHUD::ShowAppHUD(Application& app, World& world)
{
    static ImGuiComboFlags flags = 0;
    flags &= ~(ImGuiComboFlags_HeightMask_ & ~ImGuiComboFlags_HeightSmall);
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    const float PAD = 20.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = PAD;
    window_pos.y = (work_pos.y + PAD);
    window_pos_pivot.x = 0.0f;
    window_pos_pivot.y = 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    window_flags |= ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

    if (ImGui::Begin("Application Menu", nullptr, window_flags))
    {
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("Application Stats");
        ImGui::Separator();

        ImGui::Text("Frames Per Second: %.2f", 1000.0f / (app.getFrametime() * 1000.0f));
        ImGui::Text("Frametime: %.2f ms", app.getFrametime() * 1000);

            const char* items[] = { "Missile Sim Main", "Missile Tailing", "Missile Sharp Turn", "Missile Sharp Turn (High G)", "Still Missile", "Side Aspect", "Chase Cam Test", "Physics Sim", "Physics Sim Moving Platform" };

            AppHUD::item_selected_idx = LevelManager::currentLevel;
            // Pass in the preview value visible before opening the combo (it could technically be different contents or not pulled from items[])
            const char* combo_preview_value = items[item_selected_idx];
            if (ImGui::BeginCombo("Level Select", combo_preview_value, flags))
            {
                for (int n = 0; n < IM_COUNTOF(items); n++)
                {
                    const bool is_selected = (item_selected_idx == n);
                    if (ImGui::Selectable(items[n], is_selected)) {
                        item_selected_idx = n;
                        world.loadLevel(item_selected_idx);
                    }
                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                    
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();
            ImGui::Text("Target Data");
            if (world.objects.size() > 1) {
                ImGui::Text("Speed: %.1f m/s", glm::length(world.objects[1]->physicsProperties->velocity));
                if (!world.missiles.empty()) {
                    ImGui::Text("Distance: %.1f m", glm::length(world.objects[1]->position - world.missiles[0]->position));
                }
            }
            else {
                ImGui::Text("No Target!");
            }
            if (ImGui::TreeNode("Controls")) {
                ImGui::Text("W: Add 10 m/s forward");
                ImGui::Text("S: Add 10 m/s backwards");
                ImGui::Text("A: Add 10 m/s left");
                ImGui::Text("D: Add 10 m/s right");
                ImGui::Text("Left Alt: Lock/Unlock camera");
                ImGui::Text("Space: Launch Missile");
                ImGui::Text("Left Shift: Lock target");
                ImGui::Text("Up Arrow: Pitch missile up");
                ImGui::Text("Down Arrow: Pitch missile down");
                ImGui::Text("Left Arrow: Turn missile left");
                ImGui::Text("Right Arrow: Turn missile right");
                ImGui::Text("F: Target drops flare");
                ImGui::Text("R: Enable/Disable seeker");
                ImGui::Text("E: Enable Engine");
                ImGui::Text("Q: Start Tracking");
                ImGui::Text("Y: Add force up to target");
                ImGui::Text("H: Add force down to target");

                ImGui::TreePop();
            }
    }
    ImGui::End();
}