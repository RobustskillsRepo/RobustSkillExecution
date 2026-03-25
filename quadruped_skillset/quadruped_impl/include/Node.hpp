#ifndef QUADRUPED_IMPL_NODE_HPP
#define QUADRUPED_IMPL_NODE_HPP


#if defined(SKILLSET_DEBUG_MODE)
#include "quadruped_example_skillset/NodeDebug.hpp"
#define SKILLSET_NODE quadruped_example_skillset::QuadrupedExampleNodeDebug
#else
#include "quadruped_example_skillset/Node.hpp"
#define SKILLSET_NODE quadruped_example_skillset::QuadrupedExampleNode
#endif
using namespace std::chrono_literals;

class QuadrupedImplNode : public SKILLSET_NODE
{
public:
    QuadrupedImplNode() : SKILLSET_NODE("node_name", "info") {}
    //-------------------- Event Hook --------------------
    // void event_set_auto_hook();
    // void event_set_manual_hook();
    // void event_set_standing_hook();
    // void event_set_sitting_hook();
    // void event_set_poweroff_hook();
    // void event_set_poweron_hook();
    
    //-------------------- Skill Hook --------------------
    // bool skill_init_power_validate_hook();
    // void skill_init_power_start_hook();
    // void skill_init_power_on_start();
    
    // void skill_init_power_interrupt_hook();
    
    
    // bool skill_safe_poweroff_validate_hook();
    // void skill_safe_poweroff_start_hook();
    // void skill_safe_poweroff_on_start();
    
    // void skill_safe_poweroff_interrupt_hook();
    
    
    // bool skill_standup_validate_hook();
    // void skill_standup_start_hook();
    // void skill_standup_on_start();
    // void skill_standup_invariant_is_powered_hook();
    
    // void skill_standup_interrupt_hook();
    
    
    // bool skill_sitdown_validate_hook();
    // void skill_sitdown_start_hook();
    // void skill_sitdown_on_start();
    // void skill_sitdown_invariant_is_powered_hook();
    
    // void skill_sitdown_interrupt_hook();
    
    
    // bool skill_go_to_validate_hook();
    // void skill_go_to_start_hook();
    // void skill_go_to_on_start();
    // void skill_go_to_invariant_is_standing_hook();
    // void skill_go_to_invariant_is_powered_hook();
    
    // void skill_go_to_interrupt_hook();
    
    
    // bool skill_recharge_validate_hook();
    // void skill_recharge_start_hook();
    // void skill_recharge_on_start();
    // void skill_recharge_invariant_is_sitting_hook();
    
    // void skill_recharge_interrupt_hook();
    
    
    // bool skill_take_picture_validate_hook();
    // void skill_take_picture_start_hook();
    // void skill_take_picture_on_start();
    
    // void skill_take_picture_interrupt_hook();
    
    
};
#endif
