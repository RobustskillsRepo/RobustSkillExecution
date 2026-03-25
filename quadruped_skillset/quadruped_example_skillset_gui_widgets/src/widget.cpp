#include "quadruped_example_skillset_gui_widgets/widget.hpp"

#include <gui_tools/gui_tools.h>

#include <sstream>

QuadrupedExampleSkillsetWidget::QuadrupedExampleSkillsetWidget(const std::string &name, rclcpp::Node::SharedPtr node,
      bool display_data, bool display_resources, bool display_events)
    : QuadrupedExampleSkillsetClient(name, node)
    , display_data_(display_data)
    , display_resources_(display_resources)
    , display_events_(display_events)
    , event_response_timeout_(3.0)
    , subscribe_location_(false)
    , subscribe_battery_(false)
    , active_init_power_(false)
    , active_safe_poweroff_(false)
    , active_standup_(false)
    , active_sitdown_(false)
    , active_go_to_(false)
    , active_recharge_(false)
    , active_take_picture_(false)
{
    
        
    
        
    
        
    
        
    
        
        	try {
            node_->declare_parameter("quadruped_example.go_to.target.data", ""); 
        } catch (rclcpp::exceptions::ParameterAlreadyDeclaredException& e) {
            RCLCPP_WARN(node_->get_logger(), "parameter quadruped_example.go_to.target.data already declared");
        } 
        this->go_to_input_.target.data = node_->get_parameter("quadruped_example.go_to.target.data").as_string();
        

        
    
        
    
        
    
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetWidget::display_start_init_power() {
    
    active_init_power_ = true;
    
    active_safe_poweroff_ = false;
    
    active_standup_ = false;
    
    active_sitdown_ = false;
    
    active_go_to_ = false;
    
    active_recharge_ = false;
    
    active_take_picture_ = false;
    
}

void QuadrupedExampleSkillsetWidget::display_start_safe_poweroff() {
    
    active_init_power_ = false;
    
    active_safe_poweroff_ = true;
    
    active_standup_ = false;
    
    active_sitdown_ = false;
    
    active_go_to_ = false;
    
    active_recharge_ = false;
    
    active_take_picture_ = false;
    
}

void QuadrupedExampleSkillsetWidget::display_start_standup() {
    
    active_init_power_ = false;
    
    active_safe_poweroff_ = false;
    
    active_standup_ = true;
    
    active_sitdown_ = false;
    
    active_go_to_ = false;
    
    active_recharge_ = false;
    
    active_take_picture_ = false;
    
}

void QuadrupedExampleSkillsetWidget::display_start_sitdown() {
    
    active_init_power_ = false;
    
    active_safe_poweroff_ = false;
    
    active_standup_ = false;
    
    active_sitdown_ = true;
    
    active_go_to_ = false;
    
    active_recharge_ = false;
    
    active_take_picture_ = false;
    
}

void QuadrupedExampleSkillsetWidget::display_start_go_to() {
    
    active_init_power_ = false;
    
    active_safe_poweroff_ = false;
    
    active_standup_ = false;
    
    active_sitdown_ = false;
    
    active_go_to_ = true;
    
    active_recharge_ = false;
    
    active_take_picture_ = false;
    
}

void QuadrupedExampleSkillsetWidget::display_start_recharge() {
    
    active_init_power_ = false;
    
    active_safe_poweroff_ = false;
    
    active_standup_ = false;
    
    active_sitdown_ = false;
    
    active_go_to_ = false;
    
    active_recharge_ = true;
    
    active_take_picture_ = false;
    
}

void QuadrupedExampleSkillsetWidget::display_start_take_picture() {
    
    active_init_power_ = false;
    
    active_safe_poweroff_ = false;
    
    active_standup_ = false;
    
    active_sitdown_ = false;
    
    active_go_to_ = false;
    
    active_recharge_ = false;
    
    active_take_picture_ = true;
    
}



void QuadrupedExampleSkillsetWidget::event_button_set_standing() {
    if (ImGui::Button("set_standing##quadruped_example")) {
        events_["set_standing"].id = this->send_event("set_standing");
        events_["set_standing"].response = quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED;
        events_ids_[events_["set_standing"].id] = "set_standing";
    }
}

void QuadrupedExampleSkillsetWidget::event_row_set_standing() {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    event_button_set_standing();
    auto evt = events_["set_standing"];
    ImGui::TableNextColumn();
    ImVec4 color(1., 0., 0., 1.);
    std::string status = "UNKNOWN";
    switch (evt.response)
    {
    case quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS:
        status = "SUCCESS"; 
        color.x = 0.0; color.y = 1.0;
        break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED:
        status = "UNDEFINED"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE:
        status = "GUARD_FAILURE"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE:
        status = "EFFECT_FAILURE"; break;
    }
    if (this->time_since_event("set_standing") > event_response_timeout_)
        color.x = color.y = color.z = .6;
    if (!evt.id.empty())
        ImGui::TextColored(color, "%s", status.c_str());
}

void QuadrupedExampleSkillsetWidget::event_button_set_sitting() {
    if (ImGui::Button("set_sitting##quadruped_example")) {
        events_["set_sitting"].id = this->send_event("set_sitting");
        events_["set_sitting"].response = quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED;
        events_ids_[events_["set_sitting"].id] = "set_sitting";
    }
}

void QuadrupedExampleSkillsetWidget::event_row_set_sitting() {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    event_button_set_sitting();
    auto evt = events_["set_sitting"];
    ImGui::TableNextColumn();
    ImVec4 color(1., 0., 0., 1.);
    std::string status = "UNKNOWN";
    switch (evt.response)
    {
    case quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS:
        status = "SUCCESS"; 
        color.x = 0.0; color.y = 1.0;
        break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED:
        status = "UNDEFINED"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE:
        status = "GUARD_FAILURE"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE:
        status = "EFFECT_FAILURE"; break;
    }
    if (this->time_since_event("set_sitting") > event_response_timeout_)
        color.x = color.y = color.z = .6;
    if (!evt.id.empty())
        ImGui::TextColored(color, "%s", status.c_str());
}

void QuadrupedExampleSkillsetWidget::event_button_set_poweroff() {
    if (ImGui::Button("set_poweroff##quadruped_example")) {
        events_["set_poweroff"].id = this->send_event("set_poweroff");
        events_["set_poweroff"].response = quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED;
        events_ids_[events_["set_poweroff"].id] = "set_poweroff";
    }
}

void QuadrupedExampleSkillsetWidget::event_row_set_poweroff() {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    event_button_set_poweroff();
    auto evt = events_["set_poweroff"];
    ImGui::TableNextColumn();
    ImVec4 color(1., 0., 0., 1.);
    std::string status = "UNKNOWN";
    switch (evt.response)
    {
    case quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS:
        status = "SUCCESS"; 
        color.x = 0.0; color.y = 1.0;
        break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED:
        status = "UNDEFINED"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE:
        status = "GUARD_FAILURE"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE:
        status = "EFFECT_FAILURE"; break;
    }
    if (this->time_since_event("set_poweroff") > event_response_timeout_)
        color.x = color.y = color.z = .6;
    if (!evt.id.empty())
        ImGui::TextColored(color, "%s", status.c_str());
}

void QuadrupedExampleSkillsetWidget::event_button_set_poweron() {
    if (ImGui::Button("set_poweron##quadruped_example")) {
        events_["set_poweron"].id = this->send_event("set_poweron");
        events_["set_poweron"].response = quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED;
        events_ids_[events_["set_poweron"].id] = "set_poweron";
    }
}

void QuadrupedExampleSkillsetWidget::event_row_set_poweron() {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    event_button_set_poweron();
    auto evt = events_["set_poweron"];
    ImGui::TableNextColumn();
    ImVec4 color(1., 0., 0., 1.);
    std::string status = "UNKNOWN";
    switch (evt.response)
    {
    case quadruped_example_skillset_interfaces::msg::EventResponse::SUCCESS:
        status = "SUCCESS"; 
        color.x = 0.0; color.y = 1.0;
        break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::UNDEFINED:
        status = "UNDEFINED"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::GUARD_FAILURE:
        status = "GUARD_FAILURE"; break;
    case quadruped_example_skillset_interfaces::msg::EventResponse::EFFECT_FAILURE:
        status = "EFFECT_FAILURE"; break;
    }
    if (this->time_since_event("set_poweron") > event_response_timeout_)
        color.x = color.y = color.z = .6;
    if (!evt.id.empty())
        ImGui::TextColored(color, "%s", status.c_str());
}


void QuadrupedExampleSkillsetWidget::skill_response_text(int result_code) {
    std::string result;
    ImVec4 color(1., 0., 0., 1.);
    switch (result_code)
    {
    case 0: //quadruped_example_skillset_interfaces::msg::SkillResult::SUCCESS:
        result = "SUCCESS"; 
        color.x = 0.; color.y = 1.;
        break;
    case 1: //quadruped_example_skillset_interfaces::msg::SkillResult::ALREADY_RUNNING:
        result = "ALREADY RUNNING"; break;
    case 3: //quadruped_example_skillset_interfaces::msg::SkillResult::VALIDATE_FAILURE:
        result = "VALIDATE FAILURE"; break;
    case 2: //quadruped_example_skillset_interfaces::msg::SkillResult::PRECONDITION_FAILURE:
        result = "PRECONDITION FAILURE"; break;
    case 4: //quadruped_example_skillset_interfaces::msg::SkillResult::START_FAILURE:
        result = "START FAILURE"; break;
    case 5: //quadruped_example_skillset_interfaces::msg::SkillResult::INVARIANT_FAILURE:
        result = "INVARIANT FAILURE"; break;
    case 6: //quadruped_example_skillset_interfaces::msg::SkillResult::INTERRUPT:
        result = "INTERRUPT";
        color.x = 1.; color.y = 1.;
        break;
    case 7: //quadruped_example_skillset_interfaces::msg::SkillResult::FAILURE:
        result = "FAILURE"; break;            
    default:
        break;
    }
    ImGui::TextColored(color, "%s", result.c_str());
}

bool QuadrupedExampleSkillsetWidget::update_window()
{
    bool widget_alive = true;
    ImGui::Begin("QuadrupedExample Skillset Manager", &widget_alive);
    update();
    ImGui::End();
    return widget_alive;
}

void QuadrupedExampleSkillsetWidget::update()
{
    ImGui::Text("Status received: %6.1f", this->time_since_status());
    ImGui::SameLine();
    if (ImGui::Button("request status##quadruped_example")) {
        this->request_status();
    }
    
	if (this->display_data_ && ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen)) {
        
        if (ImGui::TreeNode("location")) {
            if (ImGui::Button("request data##quadruped_example")) {
                this->data_location_request();
            }
            ImGui::SameLine();
            ImGui::Checkbox("subscribe##quadruped_example-location", &subscribe_location_);
            if (this->data_location_.has_data) {
                if (ImGui::TreeNodeEx("location", ImGuiTreeNodeFlags_DefaultOpen)) {
	ImGui::Text("%s: %s", "data", this->data_location_.value.data.c_str());
	ImGui::TreePop();
}
            }
            else
                ImGui::Text("%s", "no Data");
            ImGui::TreePop();
        }
        
        if (ImGui::TreeNode("battery")) {
            if (ImGui::Button("request data##quadruped_example")) {
                this->data_battery_request();
            }
            ImGui::SameLine();
            ImGui::Checkbox("subscribe##quadruped_example-battery", &subscribe_battery_);
            if (this->data_battery_.has_data) {
                if (ImGui::TreeNodeEx("battery", ImGuiTreeNodeFlags_DefaultOpen)) {
	ImGui::Text("%s: %.6f", "data", this->data_battery_.value.data);
	ImGui::TreePop();
}
            }
            else
                ImGui::Text("%s", "no Data");
            ImGui::TreePop();
        }
        
    }
    
    
	if (this->display_resources_ && ImGui::CollapsingHeader("Resources", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginTable("#quadruped_example_resource_table", 2);
        ImGui::TableSetupColumn("name");
        ImGui::TableSetupColumn("state");
        ImGui::TableHeadersRow(); 
        for (auto r: status_.resources) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", r.name.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", r.state.c_str());
        }
        ImGui::EndTable();
    }
    
    
	if (this->display_events_ && ImGui::CollapsingHeader("Events", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::BeginTable("#quadruped_example_events", 2);
        ImGui::TableSetupColumn("#quadruped_example_event", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("#quadruped_example_response", ImGuiTableColumnFlags_WidthStretch);
        
        event_row_set_standing();
        
        event_row_set_sitting();
        
        event_row_set_poweroff();
        
        event_row_set_poweron();
        
        ImGui::EndTable();
    }
    
    
	if (ImGui::CollapsingHeader("QuadrupedExample Skills", ImGuiTreeNodeFlags_DefaultOpen)) {
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_init_power";
            ImGui::BeginDisabled();
            if (init_power_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (init_power_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (init_power_status_.state > 0));
            if (init_power_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("init_power")) {
            ImGui::Text("%s", init_power_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-init_power"))
                init_power_status_.id = this->start_init_power();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-init_power"))
                this->interrupt_init_power(init_power_status_.id);
            if (init_power_result_.id.compare(init_power_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(init_power_result_.result);
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_safe_poweroff";
            ImGui::BeginDisabled();
            if (safe_poweroff_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (safe_poweroff_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (safe_poweroff_status_.state > 0));
            if (safe_poweroff_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("safe_poweroff")) {
            ImGui::Text("%s", safe_poweroff_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-safe_poweroff"))
                safe_poweroff_status_.id = this->start_safe_poweroff();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-safe_poweroff"))
                this->interrupt_safe_poweroff(safe_poweroff_status_.id);
            if (safe_poweroff_result_.id.compare(safe_poweroff_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(safe_poweroff_result_.result);
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_standup";
            ImGui::BeginDisabled();
            if (standup_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (standup_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (standup_status_.state > 0));
            if (standup_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("standup")) {
            ImGui::Text("%s", standup_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-standup"))
                standup_status_.id = this->start_standup();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-standup"))
                this->interrupt_standup(standup_status_.id);
            if (standup_result_.id.compare(standup_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(standup_result_.result);
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_sitdown";
            ImGui::BeginDisabled();
            if (sitdown_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (sitdown_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (sitdown_status_.state > 0));
            if (sitdown_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("sitdown")) {
            ImGui::Text("%s", sitdown_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-sitdown"))
                sitdown_status_.id = this->start_sitdown();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-sitdown"))
                this->interrupt_sitdown(sitdown_status_.id);
            if (sitdown_result_.id.compare(sitdown_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(sitdown_result_.result);
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_go_to";
            ImGui::BeginDisabled();
            if (go_to_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (go_to_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (go_to_status_.state > 0));
            if (go_to_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("go_to")) {
            ImGui::Text("%s", go_to_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-go_to"))
                go_to_status_.id = this->start_go_to();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-go_to"))
                this->interrupt_go_to(go_to_status_.id);
            if (go_to_result_.id.compare(go_to_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(go_to_result_.result);
            }
            
            if (ImGui::TreeNodeEx("input", ImGuiTreeNodeFlags_DefaultOpen)) {
                
                if (ImGui::TreeNodeEx("target", ImGuiTreeNodeFlags_DefaultOpen)) {
	ImGui::InputText("data", &this->go_to_input_.target.data, 80);
	ImGui::TreePop();
}
                
                ImGui::TreePop();
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_recharge";
            ImGui::BeginDisabled();
            if (recharge_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (recharge_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (recharge_status_.state > 0));
            if (recharge_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("recharge")) {
            ImGui::Text("%s", recharge_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-recharge"))
                recharge_status_.id = this->start_recharge();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-recharge"))
                this->interrupt_recharge(recharge_status_.id);
            if (recharge_result_.id.compare(recharge_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(recharge_result_.result);
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
        {
            // Colored RadioButton
            static ImVec4 running_color({0.0, 1.0, 0.0, 1.0});
            static ImVec4 interrupting_color({1.0, 0.6, 0.0, 1.0});
            std::string label = "##color_quadruped_example_take_picture";
            ImGui::BeginDisabled();
            if (take_picture_status_.state == 1 /* RUNNING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, running_color);
            if (take_picture_status_.state == 2 /* INTERRUPTING */)
                ImGui::PushStyleColor(ImGuiCol_CheckMark, interrupting_color);
            ImGui::RadioButton(label.c_str(), (take_picture_status_.state > 0));
            if (take_picture_status_.state > 0 /* RUNNING or INTERRUPTING */)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();    
        }
        // end Colored RadioButton
        ImGui::SameLine();
        if (ImGui::TreeNodeEx("take_picture")) {
            ImGui::Text("%s", take_picture_status_.id.substr(0, 8).c_str());
            ImGui::SameLine();
            if (ImGui::Button("start##quadruped_example-take_picture"))
                take_picture_status_.id = this->start_take_picture();
            ImGui::SameLine();
            if (ImGui::Button("interrupt##quadruped_example-take_picture"))
                this->interrupt_take_picture(take_picture_status_.id);
            if (take_picture_result_.id.compare(take_picture_status_.id) == 0) {
                ImGui::SameLine();
                skill_response_text(take_picture_result_.result);
            }
            
            
            
            ImGui::Separator();
            ImGui::TreePop();
        }
        
    }
    
    ImGui::Separator();
    ImGui::Separator();
    
    if (this->active_init_power_) {
        ImGui::Text("%s", "INIT_POWER");
        
        if (ImGui::Button("START##quadruped_example-init_power", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            init_power_status_.id = this->start_init_power();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-init_power", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_init_power(init_power_status_.id);
        if (init_power_result_.id.compare(init_power_status_.id) == 0) {
            skill_response_text(init_power_result_.result);
        }
    }
    
    if (this->active_safe_poweroff_) {
        ImGui::Text("%s", "SAFE_POWEROFF");
        
        if (ImGui::Button("START##quadruped_example-safe_poweroff", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            safe_poweroff_status_.id = this->start_safe_poweroff();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-safe_poweroff", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_safe_poweroff(safe_poweroff_status_.id);
        if (safe_poweroff_result_.id.compare(safe_poweroff_status_.id) == 0) {
            skill_response_text(safe_poweroff_result_.result);
        }
    }
    
    if (this->active_standup_) {
        ImGui::Text("%s", "STANDUP");
        
        if (ImGui::Button("START##quadruped_example-standup", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            standup_status_.id = this->start_standup();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-standup", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_standup(standup_status_.id);
        if (standup_result_.id.compare(standup_status_.id) == 0) {
            skill_response_text(standup_result_.result);
        }
    }
    
    if (this->active_sitdown_) {
        ImGui::Text("%s", "SITDOWN");
        
        if (ImGui::Button("START##quadruped_example-sitdown", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            sitdown_status_.id = this->start_sitdown();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-sitdown", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_sitdown(sitdown_status_.id);
        if (sitdown_result_.id.compare(sitdown_status_.id) == 0) {
            skill_response_text(sitdown_result_.result);
        }
    }
    
    if (this->active_go_to_) {
        ImGui::Text("%s", "GO_TO");
        
        if (ImGui::TreeNodeEx(" input", ImGuiTreeNodeFlags_DefaultOpen)) {
            
            if (ImGui::TreeNodeEx("target", ImGuiTreeNodeFlags_DefaultOpen)) {
	ImGui::Text("%s: %s", "data", this->go_to_input_.target.data.c_str());
	ImGui::TreePop();
}
            
            ImGui::TreePop();
        }
        
        if (ImGui::Button("START##quadruped_example-go_to", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            go_to_status_.id = this->start_go_to();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-go_to", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_go_to(go_to_status_.id);
        if (go_to_result_.id.compare(go_to_status_.id) == 0) {
            skill_response_text(go_to_result_.result);
        }
    }
    
    if (this->active_recharge_) {
        ImGui::Text("%s", "RECHARGE");
        
        if (ImGui::Button("START##quadruped_example-recharge", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            recharge_status_.id = this->start_recharge();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-recharge", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_recharge(recharge_status_.id);
        if (recharge_result_.id.compare(recharge_status_.id) == 0) {
            skill_response_text(recharge_result_.result);
        }
    }
    
    if (this->active_take_picture_) {
        ImGui::Text("%s", "TAKE_PICTURE");
        
        if (ImGui::Button("START##quadruped_example-take_picture", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            take_picture_status_.id = this->start_take_picture();
        // ImGui::SameLine();
        if (ImGui::Button("INTERRUPT##quadruped_example-take_picture", ImVec2(10*ImGui::GetFontSize(), 2*ImGui::GetTextLineHeight())))
            this->interrupt_take_picture(take_picture_status_.id);
        if (take_picture_result_.id.compare(take_picture_status_.id) == 0) {
            skill_response_text(take_picture_result_.result);
        }
    }
    
}

void QuadrupedExampleSkillsetWidget::process() {
    
    if (subscribe_location_)
        this->create_data_location_subscription();
    else
        this->destroy_data_location_subscription();
    
    if (subscribe_battery_)
        this->create_data_battery_subscription();
    else
        this->destroy_data_battery_subscription();
    
}
