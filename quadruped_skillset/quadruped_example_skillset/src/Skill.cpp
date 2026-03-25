#include "quadruped_example_skillset/Node.hpp"

namespace quadruped_example_skillset
{
    void QuadrupedExampleNode::skills_invariants_() {
        bool effect = true;
        while (effect) {
            effect = false;
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto message = skill_init_power_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_init_power_response_pub_->publish(message);
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto message = skill_safe_poweroff_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_safe_poweroff_response_pub_->publish(message);
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto message = skill_standup_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_standup_response_pub_->publish(message);
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto message = skill_sitdown_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_sitdown_response_pub_->publish(message);
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto message = skill_go_to_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_go_to_response_pub_->publish(message);
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto message = skill_recharge_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_recharge_response_pub_->publish(message);
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto message = skill_take_picture_invariants_();
                if (message.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = message.effect;
                    skill_take_picture_response_pub_->publish(message);
                }
            }
            
        }
    }
    //-------------------------------------------------- init_power --------------------------------------------------
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_init_power_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_init_power_start_hook() {}
    void QuadrupedExampleNode::skill_init_power_on_start() {}
    //---------- Invariant ----------
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_init_power_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'init_power' interrupt");
        auto message = skill_init_power_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::INTERRUPT;
        // Stop Skill
        skill_init_power_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_init_power_interrupt_hook();
        // Post
        // Response
        skill_init_power_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_init_power_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_init_power_success_is_poweredon() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'init_power' success 'is_poweredon'");
        // Not Running -> false
        if (skill_init_power_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_init_power_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS;
        message.name = "is_poweredon";
        // Output
        // Stop Skill
        skill_init_power_state_ = SkillState::Ready;
        // Check if effects fail
        if ((
             resource_motor_state_->check_next(MotorStateState::On)
            )) {
            // Set effects
            resource_motor_state_->set_next(MotorStateState::On);
            message.effect = true;
            // Invariants
            skills_invariants_();
        }
        // Post
        // Response
        skill_init_power_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_init_power_failure_couldnot_on() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'init_power' failure 'couldnot_On'");
        // Not Running -> false
        if (skill_init_power_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_init_power_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::FAILURE;
        message.name = "couldnot_On";
        // Stop Skill
        skill_init_power_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_init_power_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse QuadrupedExampleNode::skill_init_power_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse result;
        result.id = skill_init_power_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS;
        result.is_sitting = true;
        
        result.is_not_powered = true;
        result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse QuadrupedExampleNode::skill_init_power_preconditions_() {
        auto result = skill_init_power_response_initialize_();
        bool all_success = true;
        
        // ----- precondition is_sitting -----
        result.is_sitting = (resource_spot_state_->current() == SpotStateState::Sitting);
        all_success = all_success && result.is_sitting;
        // ----- precondition is_not_powered -----
        result.is_not_powered = (resource_motor_state_->current() == MotorStateState::Off);
        all_success = all_success && result.is_not_powered;
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse QuadrupedExampleNode::skill_init_power_start_() {
        auto message = skill_init_power_response_initialize_();
        // hook
        skill_init_power_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse QuadrupedExampleNode::skill_init_power_invariants_() {
        auto message = skill_init_power_response_initialize_();
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse QuadrupedExampleNode::skill_init_power_all_invariants_() {
        auto message = skill_init_power_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_init_power_callback_(const quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'init_power' request");
        auto response = skill_init_power_response_initialize_();
        // Already Running
        if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::ALREADY_RUNNING;
            skill_init_power_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_init_power_id_ = request->id;
        // Precondition
        response = skill_init_power_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_init_power_all_invariants_();
            }
            // response
            skill_init_power_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_init_power_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::VALIDATE_FAILURE;
            skill_init_power_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_init_power_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
            // response
            skill_init_power_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_init_power_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_init_power_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_init_power_on_start();
    }

    

    void QuadrupedExampleNode::skill_init_power_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'init_power' interrupt");
        if (skill_init_power_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'init_power' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_init_power_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_init_power_interrupted_();
        mutex_.unlock();
        
    }
    //-------------------------------------------------- safe_poweroff --------------------------------------------------
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_safe_poweroff_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_safe_poweroff_start_hook() {}
    void QuadrupedExampleNode::skill_safe_poweroff_on_start() {}
    //---------- Invariant ----------
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_safe_poweroff_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'safe_poweroff' interrupt");
        auto message = skill_safe_poweroff_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::INTERRUPT;
        // Stop Skill
        skill_safe_poweroff_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_safe_poweroff_interrupt_hook();
        // Post
        // Response
        skill_safe_poweroff_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_safe_poweroff_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_safe_poweroff_success_is_poweredoff() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'safe_poweroff' success 'is_poweredoff'");
        // Not Running -> false
        if (skill_safe_poweroff_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_safe_poweroff_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS;
        message.name = "is_poweredoff";
        // Output
        // Stop Skill
        skill_safe_poweroff_state_ = SkillState::Ready;
        // Check if effects fail
        if ((
             resource_motor_state_->check_next(MotorStateState::Off)
            )) {
            // Set effects
            resource_motor_state_->set_next(MotorStateState::Off);
            message.effect = true;
            // Invariants
            skills_invariants_();
        }
        // Post
        // Response
        skill_safe_poweroff_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_safe_poweroff_failure_couldnot_off() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'safe_poweroff' failure 'couldnot_Off'");
        // Not Running -> false
        if (skill_safe_poweroff_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_safe_poweroff_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::FAILURE;
        message.name = "couldnot_Off";
        // Stop Skill
        skill_safe_poweroff_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_safe_poweroff_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse QuadrupedExampleNode::skill_safe_poweroff_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse result;
        result.id = skill_safe_poweroff_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS;
        result.is_sitting = true;
        
        result.is_powered = true;
        result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse QuadrupedExampleNode::skill_safe_poweroff_preconditions_() {
        auto result = skill_safe_poweroff_response_initialize_();
        bool all_success = true;
        
        // ----- precondition is_sitting -----
        result.is_sitting = (resource_spot_state_->current() == SpotStateState::Sitting);
        all_success = all_success && result.is_sitting;
        // ----- precondition is_powered -----
        result.is_powered = (resource_motor_state_->current() == MotorStateState::On);
        all_success = all_success && result.is_powered;
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse QuadrupedExampleNode::skill_safe_poweroff_start_() {
        auto message = skill_safe_poweroff_response_initialize_();
        // hook
        skill_safe_poweroff_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse QuadrupedExampleNode::skill_safe_poweroff_invariants_() {
        auto message = skill_safe_poweroff_response_initialize_();
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse QuadrupedExampleNode::skill_safe_poweroff_all_invariants_() {
        auto message = skill_safe_poweroff_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_safe_poweroff_callback_(const quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'safe_poweroff' request");
        auto response = skill_safe_poweroff_response_initialize_();
        // Already Running
        if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::ALREADY_RUNNING;
            skill_safe_poweroff_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_safe_poweroff_id_ = request->id;
        // Precondition
        response = skill_safe_poweroff_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_safe_poweroff_all_invariants_();
            }
            // response
            skill_safe_poweroff_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_safe_poweroff_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::VALIDATE_FAILURE;
            skill_safe_poweroff_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_safe_poweroff_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
            // response
            skill_safe_poweroff_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_safe_poweroff_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_safe_poweroff_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_safe_poweroff_on_start();
    }

    

    void QuadrupedExampleNode::skill_safe_poweroff_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'safe_poweroff' interrupt");
        if (skill_safe_poweroff_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'safe_poweroff' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_safe_poweroff_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_safe_poweroff_interrupted_();
        mutex_.unlock();
        
    }
    //-------------------------------------------------- standup --------------------------------------------------
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_standup_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_standup_start_hook() {}
    void QuadrupedExampleNode::skill_standup_on_start() {}
    //---------- Invariant ----------
    void QuadrupedExampleNode::skill_standup_invariant_is_powered_hook() {}
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_standup_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'standup' interrupt");
        auto message = skill_standup_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::INTERRUPT;
        // Stop Skill
        skill_standup_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_standup_interrupt_hook();
        // Post
        // Response
        skill_standup_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_standup_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_standup_success_is_standing() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'standup' success 'is_standing'");
        // Not Running -> false
        if (skill_standup_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_standup_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS;
        message.name = "is_standing";
        // Output
        // Stop Skill
        skill_standup_state_ = SkillState::Ready;
        // Check if effects fail
        if ((
             resource_spot_state_->check_next(SpotStateState::Standing)
            )) {
            // Set effects
            resource_spot_state_->set_next(SpotStateState::Standing);
            message.effect = true;
            // Invariants
            skills_invariants_();
        }
        // Post
        // Response
        skill_standup_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_standup_failure_couldnot_stand() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'standup' failure 'couldnot_stand'");
        // Not Running -> false
        if (skill_standup_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_standup_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::FAILURE;
        message.name = "couldnot_stand";
        // Stop Skill
        skill_standup_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_standup_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillStandupResponse QuadrupedExampleNode::skill_standup_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillStandupResponse result;
        result.id = skill_standup_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS;
        result.is_sitting = true;
        
        result.is_powered = true;
        result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillStandupResponse QuadrupedExampleNode::skill_standup_preconditions_() {
        auto result = skill_standup_response_initialize_();
        bool all_success = true;
        
        // ----- precondition is_sitting -----
        result.is_sitting = (resource_spot_state_->current() == SpotStateState::Sitting);
        all_success = all_success && result.is_sitting;
        // ----- precondition is_powered -----
        result.is_powered = (resource_motor_state_->current() == MotorStateState::On);
        all_success = all_success && result.is_powered;
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillStandupResponse QuadrupedExampleNode::skill_standup_start_() {
        auto message = skill_standup_response_initialize_();
        // hook
        skill_standup_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillStandupResponse QuadrupedExampleNode::skill_standup_invariants_() {
        auto message = skill_standup_response_initialize_();
        // ----- invariant is_powered -----
        // guard
        if (!((resource_motor_state_->current() == MotorStateState::On))) {
            message.name = "is_powered";
            message.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::INVARIANT_FAILURE;
            skill_standup_invariant_is_powered_hook();
        }
        
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillStandupResponse QuadrupedExampleNode::skill_standup_all_invariants_() {
        auto message = skill_standup_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_standup_callback_(const quadruped_example_skillset_interfaces::msg::SkillStandupRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'standup' request");
        auto response = skill_standup_response_initialize_();
        // Already Running
        if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::ALREADY_RUNNING;
            skill_standup_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_standup_id_ = request->id;
        // Precondition
        response = skill_standup_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_standup_all_invariants_();
            }
            // response
            skill_standup_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_standup_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillStandupResponse::VALIDATE_FAILURE;
            skill_standup_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_standup_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
            // response
            skill_standup_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_standup_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_standup_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_standup_on_start();
    }

    

    void QuadrupedExampleNode::skill_standup_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'standup' interrupt");
        if (skill_standup_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'standup' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_standup_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_standup_interrupted_();
        mutex_.unlock();
        
    }
    //-------------------------------------------------- sitdown --------------------------------------------------
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_sitdown_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_sitdown_start_hook() {}
    void QuadrupedExampleNode::skill_sitdown_on_start() {}
    //---------- Invariant ----------
    void QuadrupedExampleNode::skill_sitdown_invariant_is_powered_hook() {}
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_sitdown_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'sitdown' interrupt");
        auto message = skill_sitdown_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::INTERRUPT;
        // Stop Skill
        skill_sitdown_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_sitdown_interrupt_hook();
        // Post
        // Response
        skill_sitdown_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_sitdown_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_sitdown_success_is_sitting() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'sitdown' success 'is_sitting'");
        // Not Running -> false
        if (skill_sitdown_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_sitdown_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS;
        message.name = "is_sitting";
        // Output
        // Stop Skill
        skill_sitdown_state_ = SkillState::Ready;
        // Check if effects fail
        if ((
             resource_spot_state_->check_next(SpotStateState::Sitting)
            )) {
            // Set effects
            resource_spot_state_->set_next(SpotStateState::Sitting);
            message.effect = true;
            // Invariants
            skills_invariants_();
        }
        // Post
        // Response
        skill_sitdown_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_sitdown_failure_couldnot_sit() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'sitdown' failure 'couldnot_sit'");
        // Not Running -> false
        if (skill_sitdown_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_sitdown_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::FAILURE;
        message.name = "couldnot_sit";
        // Stop Skill
        skill_sitdown_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_sitdown_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillSitdownResponse QuadrupedExampleNode::skill_sitdown_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillSitdownResponse result;
        result.id = skill_sitdown_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS;
        result.is_standing = true;
        
        result.is_powered = true;
        result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillSitdownResponse QuadrupedExampleNode::skill_sitdown_preconditions_() {
        auto result = skill_sitdown_response_initialize_();
        bool all_success = true;
        
        // ----- precondition is_standing -----
        result.is_standing = (resource_spot_state_->current() == SpotStateState::Standing);
        all_success = all_success && result.is_standing;
        // ----- precondition is_powered -----
        result.is_powered = (resource_motor_state_->current() == MotorStateState::On);
        all_success = all_success && result.is_powered;
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillSitdownResponse QuadrupedExampleNode::skill_sitdown_start_() {
        auto message = skill_sitdown_response_initialize_();
        // hook
        skill_sitdown_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillSitdownResponse QuadrupedExampleNode::skill_sitdown_invariants_() {
        auto message = skill_sitdown_response_initialize_();
        // ----- invariant is_powered -----
        // guard
        if (!((resource_motor_state_->current() == MotorStateState::On))) {
            message.name = "is_powered";
            message.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::INVARIANT_FAILURE;
            skill_sitdown_invariant_is_powered_hook();
        }
        
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillSitdownResponse QuadrupedExampleNode::skill_sitdown_all_invariants_() {
        auto message = skill_sitdown_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_sitdown_callback_(const quadruped_example_skillset_interfaces::msg::SkillSitdownRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'sitdown' request");
        auto response = skill_sitdown_response_initialize_();
        // Already Running
        if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::ALREADY_RUNNING;
            skill_sitdown_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_sitdown_id_ = request->id;
        // Precondition
        response = skill_sitdown_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_sitdown_all_invariants_();
            }
            // response
            skill_sitdown_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_sitdown_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::VALIDATE_FAILURE;
            skill_sitdown_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_sitdown_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
            // response
            skill_sitdown_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_sitdown_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_sitdown_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_sitdown_on_start();
    }

    

    void QuadrupedExampleNode::skill_sitdown_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'sitdown' interrupt");
        if (skill_sitdown_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'sitdown' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_sitdown_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_sitdown_interrupted_();
        mutex_.unlock();
        
    }
    //-------------------------------------------------- go_to --------------------------------------------------
    const quadruped_example_skillset_interfaces::msg::SkillGoToInput::SharedPtr QuadrupedExampleNode::skill_go_to_input() const
    {
        return skill_go_to_input_;
    }
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_go_to_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_go_to_start_hook() {}
    void QuadrupedExampleNode::skill_go_to_on_start() {}
    //---------- Invariant ----------
    void QuadrupedExampleNode::skill_go_to_invariant_is_standing_hook() {}
    void QuadrupedExampleNode::skill_go_to_invariant_is_powered_hook() {}
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_go_to_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'go_to' interrupt");
        auto message = skill_go_to_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::INTERRUPT;
        // Stop Skill
        skill_go_to_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_go_to_interrupt_hook();
        // Post
        // Response
        skill_go_to_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_go_to_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_go_to_success_is_arrived() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'go_to' success 'is_arrived'");
        // Not Running -> false
        if (skill_go_to_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_go_to_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS;
        message.name = "is_arrived";
        // Output
        // Stop Skill
        skill_go_to_state_ = SkillState::Ready;
        // Check if effects fail
        // Post
        // Response
        skill_go_to_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_go_to_failure_not_arrived() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'go_to' failure 'not_arrived'");
        // Not Running -> false
        if (skill_go_to_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_go_to_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::FAILURE;
        message.name = "not_arrived";
        // Stop Skill
        skill_go_to_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_go_to_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillGoToResponse QuadrupedExampleNode::skill_go_to_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillGoToResponse result;
        result.id = skill_go_to_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS;
        result.is_standing = true;
        
        result.is_powered = true;
        result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillGoToResponse QuadrupedExampleNode::skill_go_to_preconditions_() {
        auto result = skill_go_to_response_initialize_();
        bool all_success = true;
        
        // ----- precondition is_standing -----
        result.is_standing = (resource_spot_state_->current() == SpotStateState::Standing);
        all_success = all_success && result.is_standing;
        // ----- precondition is_powered -----
        result.is_powered = (resource_motor_state_->current() == MotorStateState::On);
        all_success = all_success && result.is_powered;
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillGoToResponse QuadrupedExampleNode::skill_go_to_start_() {
        auto message = skill_go_to_response_initialize_();
        // hook
        skill_go_to_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillGoToResponse QuadrupedExampleNode::skill_go_to_invariants_() {
        auto message = skill_go_to_response_initialize_();
        // ----- invariant is_standing -----
        // guard
        if (!((resource_spot_state_->current() == SpotStateState::Standing))) {
            message.name = "is_standing";
            message.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::INVARIANT_FAILURE;
            skill_go_to_invariant_is_standing_hook();
        }
        
        // ----- invariant is_powered -----
        // guard
        if (!((resource_motor_state_->current() == MotorStateState::On))) {
            message.name = "is_powered";
            message.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::INVARIANT_FAILURE;
            skill_go_to_invariant_is_powered_hook();
        }
        
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillGoToResponse QuadrupedExampleNode::skill_go_to_all_invariants_() {
        auto message = skill_go_to_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_go_to_callback_(const quadruped_example_skillset_interfaces::msg::SkillGoToRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'go_to' request");
        auto response = skill_go_to_response_initialize_();
        // Already Running
        if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::ALREADY_RUNNING;
            skill_go_to_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_go_to_id_ = request->id;
        // Input
        skill_go_to_input_ = std::make_shared<quadruped_example_skillset_interfaces::msg::SkillGoToInput>(request->input);
        // Precondition
        response = skill_go_to_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_go_to_all_invariants_();
            }
            // response
            skill_go_to_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_go_to_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillGoToResponse::VALIDATE_FAILURE;
            skill_go_to_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_go_to_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
            // response
            skill_go_to_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_go_to_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_go_to_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_go_to_on_start();
    }

    

    void QuadrupedExampleNode::skill_go_to_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'go_to' interrupt");
        if (skill_go_to_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'go_to' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_go_to_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_go_to_interrupted_();
        mutex_.unlock();
        
    }
    //-------------------------------------------------- recharge --------------------------------------------------
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_recharge_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_recharge_start_hook() {}
    void QuadrupedExampleNode::skill_recharge_on_start() {}
    //---------- Invariant ----------
    void QuadrupedExampleNode::skill_recharge_invariant_is_sitting_hook() {}
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_recharge_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'recharge' interrupt");
        auto message = skill_recharge_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::INTERRUPT;
        // Stop Skill
        skill_recharge_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_recharge_interrupt_hook();
        // Post
        // Response
        skill_recharge_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_recharge_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_recharge_success_has_charged() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'recharge' success 'has_charged'");
        // Not Running -> false
        if (skill_recharge_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_recharge_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS;
        message.name = "has_charged";
        // Output
        // Stop Skill
        skill_recharge_state_ = SkillState::Ready;
        // Check if effects fail
        // Post
        // Response
        skill_recharge_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_recharge_failure_couldnot_charge() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'recharge' failure 'couldnot_charge'");
        // Not Running -> false
        if (skill_recharge_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_recharge_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::FAILURE;
        message.name = "couldnot_charge";
        // Stop Skill
        skill_recharge_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_recharge_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillRechargeResponse QuadrupedExampleNode::skill_recharge_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillRechargeResponse result;
        result.id = skill_recharge_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS;
        result.is_sitting = true;
        result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillRechargeResponse QuadrupedExampleNode::skill_recharge_preconditions_() {
        auto result = skill_recharge_response_initialize_();
        bool all_success = true;
        
        // ----- precondition is_sitting -----
        result.is_sitting = (resource_spot_state_->current() == SpotStateState::Sitting);
        all_success = all_success && result.is_sitting;
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillRechargeResponse QuadrupedExampleNode::skill_recharge_start_() {
        auto message = skill_recharge_response_initialize_();
        // hook
        skill_recharge_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillRechargeResponse QuadrupedExampleNode::skill_recharge_invariants_() {
        auto message = skill_recharge_response_initialize_();
        // ----- invariant is_sitting -----
        // guard
        if (!((resource_spot_state_->current() == SpotStateState::Sitting))) {
            message.name = "is_sitting";
            message.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::INVARIANT_FAILURE;
            skill_recharge_invariant_is_sitting_hook();
        }
        
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillRechargeResponse QuadrupedExampleNode::skill_recharge_all_invariants_() {
        auto message = skill_recharge_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_recharge_callback_(const quadruped_example_skillset_interfaces::msg::SkillRechargeRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'recharge' request");
        auto response = skill_recharge_response_initialize_();
        // Already Running
        if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::ALREADY_RUNNING;
            skill_recharge_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_recharge_id_ = request->id;
        // Precondition
        response = skill_recharge_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_recharge_all_invariants_();
            }
            // response
            skill_recharge_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_recharge_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::VALIDATE_FAILURE;
            skill_recharge_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_recharge_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
            // response
            skill_recharge_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_recharge_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_recharge_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_recharge_on_start();
    }

    

    void QuadrupedExampleNode::skill_recharge_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'recharge' interrupt");
        if (skill_recharge_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'recharge' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_recharge_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_recharge_interrupted_();
        mutex_.unlock();
        
    }
    //-------------------------------------------------- take_picture --------------------------------------------------
    //---------- Validate ----------
    bool QuadrupedExampleNode::skill_take_picture_validate_hook() {
        return true;
    }
    //---------- Start ----------
    void QuadrupedExampleNode::skill_take_picture_start_hook() {}
    void QuadrupedExampleNode::skill_take_picture_on_start() {}
    //---------- Invariant ----------
    //---------- Interrupt ----------
    void QuadrupedExampleNode::skill_take_picture_interrupted_() {
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'take_picture' interrupt");
        auto message = skill_take_picture_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::INTERRUPT;
        // Stop Skill
        skill_take_picture_state_ = SkillState::Ready;
        // Check effects
        // hook
        skill_take_picture_interrupt_hook();
        // Post
        // Response
        skill_take_picture_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
    }
    void QuadrupedExampleNode::skill_take_picture_interrupt_hook() {}
    //---------- Success ----------
    bool QuadrupedExampleNode::skill_take_picture_success_pic_taken() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'take_picture' success 'pic_taken'");
        // Not Running -> false
        if (skill_take_picture_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_take_picture_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS;
        message.name = "pic_taken";
        // Output
        // Stop Skill
        skill_take_picture_state_ = SkillState::Ready;
        // Check if effects fail
        // Post
        // Response
        skill_take_picture_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }
    //---------- Failure ----------
    bool QuadrupedExampleNode::skill_take_picture_failure_pic_failed() {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'take_picture' failure 'pic_failed'");
        // Not Running -> false
        if (skill_take_picture_state_ != SkillState::Running) {
            mutex_.unlock();
            return false;
        }
        auto message = skill_take_picture_response_initialize_();
        message.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::FAILURE;
        message.name = "pic_failed";
        // Stop Skill
        skill_take_picture_state_ = SkillState::Ready;
        // Check effects
        // Post
        // Response
        skill_take_picture_response_pub_->publish(message);
        // Status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        return true;
    }//------------------------- Inner Function -------------------------
    quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse QuadrupedExampleNode::skill_take_picture_response_initialize_() const {
        quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse result;
        result.id = skill_take_picture_id_;
        result.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS;result.name = "";
        result.effect = false;
        result.postcondition = true;
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse QuadrupedExampleNode::skill_take_picture_preconditions_() {
        auto result = skill_take_picture_response_initialize_();
        bool all_success = true;
        
        if (!all_success) {
            result.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::PRECONDITION_FAILURE;
        }
        return result;
    }

    quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse QuadrupedExampleNode::skill_take_picture_start_() {
        auto message = skill_take_picture_response_initialize_();
        // hook
        skill_take_picture_start_hook();
        // set effects
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse QuadrupedExampleNode::skill_take_picture_invariants_() {
        auto message = skill_take_picture_response_initialize_();
        return message;
    }

    quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse QuadrupedExampleNode::skill_take_picture_all_invariants_() {
        auto message = skill_take_picture_response_initialize_();

        bool effect = true;
        while (effect) {
            effect = false;
            
            if (skill_init_power_state_ == SkillState::Running || skill_init_power_state_ == SkillState::Interrupting) {
                auto response = skill_init_power_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_init_power_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_init_power_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_safe_poweroff_state_ == SkillState::Running || skill_safe_poweroff_state_ == SkillState::Interrupting) {
                auto response = skill_safe_poweroff_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_safe_poweroff_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_safe_poweroff_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_standup_state_ == SkillState::Running || skill_standup_state_ == SkillState::Interrupting) {
                auto response = skill_standup_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_standup_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_standup_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_sitdown_state_ == SkillState::Running || skill_sitdown_state_ == SkillState::Interrupting) {
                auto response = skill_sitdown_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_sitdown_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_sitdown_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_go_to_state_ == SkillState::Running || skill_go_to_state_ == SkillState::Interrupting) {
                auto response = skill_go_to_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_go_to_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_go_to_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_recharge_state_ == SkillState::Running || skill_recharge_state_ == SkillState::Interrupting) {
                auto response = skill_recharge_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_recharge_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_recharge_response_pub_->publish(response);
                    
                }
            }
            
            if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
                auto response = skill_take_picture_invariants_();
                if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
                    // Stop Skill
                    skill_take_picture_state_ = SkillState::Ready;
                    // Effects
                    effect = response.effect;
                    // Response
                    skill_take_picture_response_pub_->publish(response);
                    
                    message = response;
                    
                }
            }
            
        }
        return message;
    }

    //------------------------- Callback -------------------------

    void QuadrupedExampleNode::skill_take_picture_callback_(const quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest::UniquePtr request) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'take_picture' request");
        auto response = skill_take_picture_response_initialize_();
        // Already Running
        if (skill_take_picture_state_ == SkillState::Running || skill_take_picture_state_ == SkillState::Interrupting) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::ALREADY_RUNNING;
            skill_take_picture_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Skill id
        skill_take_picture_id_ = request->id;
        // Precondition
        response = skill_take_picture_preconditions_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
            // other invariants
            if (response.effect) {
                skill_take_picture_all_invariants_();
            }
            // response
            skill_take_picture_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Validate
        if (!skill_take_picture_validate_hook()) {
            // response
            response.result = quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::VALIDATE_FAILURE;
            skill_take_picture_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Start
        response = skill_take_picture_start_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
            // response
            skill_take_picture_response_pub_->publish(response);
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // Run
        skill_take_picture_state_ = SkillState::Running;
        // Check Invariant (Loop)
        // TODO: check if effect ?
        response = skill_take_picture_all_invariants_();
        if (response.result != quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SUCCESS) {
            // status
            auto status_message = status_();
            status_pub_->publish(status_message);
            mutex_.unlock();
            return;
        }
        // status
        auto status_message = status_();
        status_pub_->publish(status_message);
        mutex_.unlock();
        // callback
        this->skill_take_picture_on_start();
    }

    

    void QuadrupedExampleNode::skill_take_picture_interrupt_callback_(const quadruped_example_skillset_interfaces::msg::SkillInterrupt::UniquePtr msg) {
        mutex_.lock();
        RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'take_picture' interrupt");
        if (skill_take_picture_id_ != msg->id) {
            RCLCPP_DEBUG(this->get_logger(), "skillset 'quadruped_example' skill 'take_picture' worng id");
            mutex_.unlock();
            return;    
        }
        // Not Running -> finish
        if (skill_take_picture_state_ != SkillState::Running) {            
            mutex_.unlock();
            // TODO return something ?
            return;    
        }
        // if Interrupting
        skill_take_picture_interrupted_();
        mutex_.unlock();
        
    }
}
