#ifndef QUADRUPED_EXAMPLE_RESOURCE_HPP
#define QUADRUPED_EXAMPLE_RESOURCE_HPP

#include <string>
#include <vector>
#include <ostream>

namespace quadruped_example_skillset
{
    template <typename T>
    class Arc
    {
    public:
        Arc(T src, T dst) : src_(src), dst_(dst) {}
        inline T src() const { return src_; }
        inline T dst() const { return dst_; }
    private:
        T src_;
        T dst_;
    };

    template <typename T>
    class Resource
    {
    public:
        Resource(T initial) : current_(initial) {}
        virtual ~Resource() {};
        //
        virtual std::string name() const = 0;
        inline T current() const { return current_; };
        virtual std::vector<T> states() const = 0;
        virtual std::vector<Arc<T>> transitions() const = 0;
        virtual bool check_next(T dst) const = 0;
        //
        void set_next(T dst) {
            current_ = dst;
        }
    private:
        T current_;
    };

    //------------------------- motor_state -------------------------
    enum class MotorStateState
    {
        Off, On, 
    };

    class MotorState : public Resource<MotorStateState>
    {
    public:
        MotorState() : Resource(MotorStateState::Off) {}
        ~MotorState() {}

        inline std::string name() const { return std::string("motor_state"); };
        std::vector<MotorStateState> states() const;
        std::vector<Arc<MotorStateState>> transitions() const;
        bool check_next(MotorStateState dst) const;
    };

    //------------------------- spot_state -------------------------
    enum class SpotStateState
    {
        Sitting, Standing, 
    };

    class SpotState : public Resource<SpotStateState>
    {
    public:
        SpotState() : Resource(SpotStateState::Sitting) {}
        ~SpotState() {}

        inline std::string name() const { return std::string("spot_state"); };
        std::vector<SpotStateState> states() const;
        std::vector<Arc<SpotStateState>> transitions() const;
        bool check_next(SpotStateState dst) const;
    };

    
}


std::string to_string(const quadruped_example_skillset::MotorStateState &x);
std::ostream &operator<<(std::ostream &out, const quadruped_example_skillset::MotorStateState &x);

std::string to_string(const quadruped_example_skillset::SpotStateState &x);
std::ostream &operator<<(std::ostream &out, const quadruped_example_skillset::SpotStateState &x);


#endif
