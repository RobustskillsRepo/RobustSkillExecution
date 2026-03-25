#include "quadruped_example_skillset/Resource.hpp"

namespace quadruped_example_skillset
{
    //-------------------------------------------------- motor_state --------------------------------------------------

    std::vector<MotorStateState> MotorState::states() const
    {
        return std::vector<MotorStateState>{
            MotorStateState::Off, MotorStateState::On, 
        };
    }

    std::vector<Arc<MotorStateState>> MotorState::transitions() const
    {
        return std::vector<Arc<MotorStateState>>{
            Arc<MotorStateState>(MotorStateState::Off, MotorStateState::On), 
            Arc<MotorStateState>(MotorStateState::On, MotorStateState::Off), 
            };
    }

    bool MotorState::check_next(MotorStateState dst) const
    {
        if (current() == dst) {
            return true;
        }

        switch (current()) {
        case MotorStateState::Off:
            return (dst == MotorStateState::On);
        case MotorStateState::On:
            return (dst == MotorStateState::Off);
        default: return false;
        }
    
    }
    //-------------------------------------------------- spot_state --------------------------------------------------

    std::vector<SpotStateState> SpotState::states() const
    {
        return std::vector<SpotStateState>{
            SpotStateState::Sitting, SpotStateState::Standing, 
        };
    }

    std::vector<Arc<SpotStateState>> SpotState::transitions() const
    {
        return std::vector<Arc<SpotStateState>>{
            Arc<SpotStateState>(SpotStateState::Sitting, SpotStateState::Standing), 
            Arc<SpotStateState>(SpotStateState::Standing, SpotStateState::Sitting), 
            };
    }

    bool SpotState::check_next(SpotStateState dst) const
    {
        if (current() == dst) {
            return true;
        }

        switch (current()) {
        case SpotStateState::Sitting:
            return (dst == SpotStateState::Standing);
        case SpotStateState::Standing:
            return (dst == SpotStateState::Sitting);
        default: return false;
        }
    
    }
    
}


std::string to_string(const quadruped_example_skillset::MotorStateState &x)
{
    switch (x)
    {
    case quadruped_example_skillset::MotorStateState::Off:
        return "Off";
    case quadruped_example_skillset::MotorStateState::On:
        return "On";
    }
    return "";
}

std::ostream &operator<<(std::ostream &out, const quadruped_example_skillset::MotorStateState &x)
{
    switch (x)
    {
    case quadruped_example_skillset::MotorStateState::Off:
        out << "Off";
        break;
    case quadruped_example_skillset::MotorStateState::On:
        out << "On";
        break;
    }
    return out;
}

std::string to_string(const quadruped_example_skillset::SpotStateState &x)
{
    switch (x)
    {
    case quadruped_example_skillset::SpotStateState::Sitting:
        return "Sitting";
    case quadruped_example_skillset::SpotStateState::Standing:
        return "Standing";
    }
    return "";
}

std::ostream &operator<<(std::ostream &out, const quadruped_example_skillset::SpotStateState &x)
{
    switch (x)
    {
    case quadruped_example_skillset::SpotStateState::Sitting:
        out << "Sitting";
        break;
    case quadruped_example_skillset::SpotStateState::Standing:
        out << "Standing";
        break;
    }
    return out;
}

