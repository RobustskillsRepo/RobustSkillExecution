
#include <random>
#include "quadruped_example_skillset_gui_widgets/client.hpp"

QuadrupedExampleSkillsetClient::QuadrupedExampleSkillsetClient(const std::string &name, rclcpp::Node::SharedPtr node)
    : name_(name)
    , node_(node)
    , qos_best_(1), qos_reliable_(1)
{
    //----- resource init
    
    resource_state_["motor_state"] = "";
    
    resource_state_["spot_state"] = "";
    
    qos_best_.best_effort();
    qos_best_.durability_volatile();
    qos_reliable_.reliable();
    qos_reliable_.durability_volatile();

    status_pub_ = node_->create_publisher<std_msgs::msg::Empty>(
        name+"/quadruped_example_skillset/status_request", qos_reliable_);
    status_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillsetStatus>(
        name+"/quadruped_example_skillset/status", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::status_callback_, this, std::placeholders::_1));

    event_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::EventRequest>(
        name+"/quadruped_example_skillset/event_request", qos_reliable_);
    event_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::EventResponse>(
        name+"/quadruped_example_skillset/event_response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::event_callback_, this, std::placeholders::_1));
    
    data_location_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::DataRequest>(
        name+"/quadruped_example_skillset/data/location/request", qos_reliable_);
    data_location_response_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::DataLocationResponse>(
        name+"/quadruped_example_skillset/data/location/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::data_location_response_callback_, this, std::placeholders::_1));
    
    data_battery_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::DataRequest>(
        name+"/quadruped_example_skillset/data/battery/request", qos_reliable_);
    data_battery_response_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::DataBatteryResponse>(
        name+"/quadruped_example_skillset/data/battery/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::data_battery_response_callback_, this, std::placeholders::_1));
    
    
    init_power_result_.id = "00000000";
    init_power_status_.id = "";
    init_power_status_.state = quadruped_example_skillset_interfaces::msg::SkillInitPowerStatus::READY;
    init_power_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest>(
        name+"/quadruped_example_skillset/skill/init_power/request", qos_reliable_);
    init_power_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/init_power/interrupt", qos_reliable_);
    
    init_power_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse>(
        name+"/quadruped_example_skillset/skill/init_power/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::init_power_response_callback, this, std::placeholders::_1));
    
    safe_poweroff_result_.id = "00000000";
    safe_poweroff_status_.id = "";
    safe_poweroff_status_.state = quadruped_example_skillset_interfaces::msg::SkillSafePoweroffStatus::READY;
    safe_poweroff_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest>(
        name+"/quadruped_example_skillset/skill/safe_poweroff/request", qos_reliable_);
    safe_poweroff_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/safe_poweroff/interrupt", qos_reliable_);
    
    safe_poweroff_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse>(
        name+"/quadruped_example_skillset/skill/safe_poweroff/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::safe_poweroff_response_callback, this, std::placeholders::_1));
    
    standup_result_.id = "00000000";
    standup_status_.id = "";
    standup_status_.state = quadruped_example_skillset_interfaces::msg::SkillStandupStatus::READY;
    standup_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillStandupRequest>(
        name+"/quadruped_example_skillset/skill/standup/request", qos_reliable_);
    standup_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/standup/interrupt", qos_reliable_);
    
    standup_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillStandupResponse>(
        name+"/quadruped_example_skillset/skill/standup/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::standup_response_callback, this, std::placeholders::_1));
    
    sitdown_result_.id = "00000000";
    sitdown_status_.id = "";
    sitdown_status_.state = quadruped_example_skillset_interfaces::msg::SkillSitdownStatus::READY;
    sitdown_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillSitdownRequest>(
        name+"/quadruped_example_skillset/skill/sitdown/request", qos_reliable_);
    sitdown_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/sitdown/interrupt", qos_reliable_);
    
    sitdown_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillSitdownResponse>(
        name+"/quadruped_example_skillset/skill/sitdown/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::sitdown_response_callback, this, std::placeholders::_1));
    
    go_to_result_.id = "00000000";
    go_to_status_.id = "";
    go_to_status_.state = quadruped_example_skillset_interfaces::msg::SkillGoToStatus::READY;
    go_to_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillGoToRequest>(
        name+"/quadruped_example_skillset/skill/go_to/request", qos_reliable_);
    go_to_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/go_to/interrupt", qos_reliable_);
    
    go_to_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillGoToResponse>(
        name+"/quadruped_example_skillset/skill/go_to/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::go_to_response_callback, this, std::placeholders::_1));
    
    recharge_result_.id = "00000000";
    recharge_status_.id = "";
    recharge_status_.state = quadruped_example_skillset_interfaces::msg::SkillRechargeStatus::READY;
    recharge_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillRechargeRequest>(
        name+"/quadruped_example_skillset/skill/recharge/request", qos_reliable_);
    recharge_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/recharge/interrupt", qos_reliable_);
    
    recharge_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillRechargeResponse>(
        name+"/quadruped_example_skillset/skill/recharge/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::recharge_response_callback, this, std::placeholders::_1));
    
    take_picture_result_.id = "00000000";
    take_picture_status_.id = "";
    take_picture_status_.state = quadruped_example_skillset_interfaces::msg::SkillTakePictureStatus::READY;
    take_picture_request_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest>(
        name+"/quadruped_example_skillset/skill/take_picture/request", qos_reliable_);
    take_picture_interrupt_pub_ = node_->create_publisher<quadruped_example_skillset_interfaces::msg::SkillInterrupt>(
        name+"/quadruped_example_skillset/skill/take_picture/interrupt", qos_reliable_);
    
    take_picture_response_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse>(
        name+"/quadruped_example_skillset/skill/take_picture/response", qos_reliable_, std::bind(&QuadrupedExampleSkillsetClient::take_picture_response_callback, this, std::placeholders::_1));
    
    auto now = node_->get_clock()->now();
    
    events_stamps_["set_standing"] = now;
    
    events_stamps_["set_sitting"] = now;
    
    events_stamps_["set_poweroff"] = now;
    
    events_stamps_["set_poweron"] = now;
    
    
    quadruped_example_skillset_interfaces::msg::ResourceState r_motor_state;
    r_motor_state.name = "motor_state"; 
    r_motor_state.state = "";
    status_.resources.push_back(r_motor_state);
    
    quadruped_example_skillset_interfaces::msg::ResourceState r_spot_state;
    r_spot_state.name = "spot_state"; 
    r_spot_state.state = "";
    status_.resources.push_back(r_spot_state);
    
    status_pub_->publish(std_msgs::msg::Empty());
}

std::string QuadrupedExampleSkillsetClient::generate_id() const {
    std::random_device rd;
    auto seed_data = std::array<int, std::mt19937::state_size> {};
    std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    std::mt19937 generator(seq);

    std::array<uint8_t, 16> data{ { 0 } };
    std::uniform_int_distribution<uint32_t>  distribution;
    uint8_t bytes[16];
    for (int i = 0; i < 16; i += 4)
        *reinterpret_cast<uint32_t*>(bytes + i) = distribution(generator);
    // variant must be 10xxxxxx
    bytes[8] &= 0xBF;
    bytes[8] |= 0x80;
    // version must be 0100xxxx
    bytes[6] &= 0x4F;
    bytes[6] |= 0x40;
    std::copy(std::begin(bytes), std::end(bytes), std::begin(data));
    std::stringstream s;
    // manipulate stream as needed
    s << std::hex << std::setfill(static_cast<char>('0'))
        << std::setw(2) << (int)data[0]
        << std::setw(2) << (int)data[1]
        << std::setw(2) << (int)data[2]
        << std::setw(2) << (int)data[3]
        << '-'
        << std::setw(2) << (int)data[4]
        << std::setw(2) << (int)data[5]
        << '-'
        << std::setw(2) << (int)data[6]
        << std::setw(2) << (int)data[7]
        << '-'
        << std::setw(2) << (int)data[8]
        << std::setw(2) << (int)data[9]
        << '-'
        << std::setw(2) << (int)data[10]
        << std::setw(2) << (int)data[11]
        << std::setw(2) << (int)data[12]
        << std::setw(2) << (int)data[13]
        << std::setw(2) << (int)data[14]
        << std::setw(2) << (int)data[15];
    return s.str();
}

void QuadrupedExampleSkillsetClient::status_callback_(const quadruped_example_skillset_interfaces::msg::SkillsetStatus::SharedPtr msg) {
    RCLCPP_DEBUG(node_->get_logger(), "[%s] received skillset status", name_.c_str());
    this->status_ = *msg;
    for (auto r: msg->resources)
        resource_state_[r.name] = r.state;
    init_power_status_ = msg->skill_init_power;
    safe_poweroff_status_ = msg->skill_safe_poweroff;
    standup_status_ = msg->skill_standup;
    sitdown_status_ = msg->skill_sitdown;
    go_to_status_ = msg->skill_go_to;
    recharge_status_ = msg->skill_recharge;
    take_picture_status_ = msg->skill_take_picture;
    
}

double QuadrupedExampleSkillsetClient::time_since_status() {
    rclcpp::Time now = node_->get_clock()->now();
    rclcpp::Time stamp = status_.stamp;
    return (now-stamp).seconds();
}

void QuadrupedExampleSkillsetClient::request_status() {
    RCLCPP_INFO(node_->get_logger(), "[%s] Request status", name_.c_str());
    status_pub_->publish(std_msgs::msg::Empty());
}

//-----------------------------------------------------------------------------

std::string QuadrupedExampleSkillsetClient::send_event(std::string event) {
    quadruped_example_skillset_interfaces::msg::EventRequest request;
    request.id = generate_id();
    request.name = event;
    RCLCPP_INFO(node_->get_logger(), "[%s] send event %s (%s)", 
        name_.c_str(), request.name.c_str(), request.id.c_str());
    this->event_pub_->publish(request);
    return request.id;
}

void QuadrupedExampleSkillsetClient::event_callback_(const quadruped_example_skillset_interfaces::msg::EventResponse::SharedPtr msg) {
    RCLCPP_INFO(node_->get_logger(), "[%s] received event %s response %d", 
        name_.c_str(), msg->id.c_str(), msg->response);
    events_[events_ids_[msg->id]] = *msg;
    events_stamps_[events_ids_[msg->id]] = node_->get_clock()->now();
}

double QuadrupedExampleSkillsetClient::time_since_event(std::string event) const {
    auto now = node_->get_clock()->now();
    return (now - events_stamps_.at(event)).seconds();
}


//-----------------------------------------------------------------------------
void QuadrupedExampleSkillsetClient::data_location_response_callback_(const quadruped_example_skillset_interfaces::msg::DataLocationResponse::SharedPtr msg) {
    RCLCPP_INFO(node_->get_logger(), "[%s] received data 'location' response %s", 
        name_.c_str(), msg->id.c_str());
    this->data_location_.has_data = msg->has_data;
    this->data_location_.value = msg->value;
}

void QuadrupedExampleSkillsetClient::data_location_callback_(const quadruped_example_skillset_interfaces::msg::DataLocation::SharedPtr msg) {
    RCLCPP_DEBUG(node_->get_logger(), "[%s] received data 'location'", name_.c_str());
    this->data_location_.has_data = true;
    this->data_location_.value = msg->value;
}

std::string QuadrupedExampleSkillsetClient::data_location_request() {
    quadruped_example_skillset_interfaces::msg::DataRequest request;
    request.id = generate_id();
    this->data_location_pub_->publish(request);
    return request.id;
}

void QuadrupedExampleSkillsetClient::create_data_location_subscription() {
    if (! data_location_sub_) {
        RCLCPP_INFO(node_->get_logger(), "[%s] create subsription to data 'location'", name_.c_str());
        data_location_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::DataLocation>(
            name_+"/quadruped_example_skillset/data/location", qos_reliable_, 
            std::bind(&QuadrupedExampleSkillsetClient::data_location_callback_, this, std::placeholders::_1));
    }
}

void QuadrupedExampleSkillsetClient::destroy_data_location_subscription() {
    if (data_location_sub_) {
        RCLCPP_INFO(node_->get_logger(), "[%s] reset subsription to data 'location'", name_.c_str());
        data_location_sub_.reset();
    }
}

//-----------------------------------------------------------------------------
void QuadrupedExampleSkillsetClient::data_battery_response_callback_(const quadruped_example_skillset_interfaces::msg::DataBatteryResponse::SharedPtr msg) {
    RCLCPP_INFO(node_->get_logger(), "[%s] received data 'battery' response %s", 
        name_.c_str(), msg->id.c_str());
    this->data_battery_.has_data = msg->has_data;
    this->data_battery_.value = msg->value;
}

void QuadrupedExampleSkillsetClient::data_battery_callback_(const quadruped_example_skillset_interfaces::msg::DataBattery::SharedPtr msg) {
    RCLCPP_DEBUG(node_->get_logger(), "[%s] received data 'battery'", name_.c_str());
    this->data_battery_.has_data = true;
    this->data_battery_.value = msg->value;
}

std::string QuadrupedExampleSkillsetClient::data_battery_request() {
    quadruped_example_skillset_interfaces::msg::DataRequest request;
    request.id = generate_id();
    this->data_battery_pub_->publish(request);
    return request.id;
}

void QuadrupedExampleSkillsetClient::create_data_battery_subscription() {
    if (! data_battery_sub_) {
        RCLCPP_INFO(node_->get_logger(), "[%s] create subsription to data 'battery'", name_.c_str());
        data_battery_sub_ = node_->create_subscription<quadruped_example_skillset_interfaces::msg::DataBattery>(
            name_+"/quadruped_example_skillset/data/battery", qos_reliable_, 
            std::bind(&QuadrupedExampleSkillsetClient::data_battery_callback_, this, std::placeholders::_1));
    }
}

void QuadrupedExampleSkillsetClient::destroy_data_battery_subscription() {
    if (data_battery_sub_) {
        RCLCPP_INFO(node_->get_logger(), "[%s] reset subsription to data 'battery'", name_.c_str());
        data_battery_sub_.reset();
    }
}



//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::init_power_response_callback(const quadruped_example_skillset_interfaces::msg::SkillInitPowerResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received init_power %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->init_power_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_init_power() {
    quadruped_example_skillset_interfaces::msg::SkillInitPowerRequest msg;
    
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill init_power %s", 
        name_.c_str(), msg.id.c_str());
    init_power_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_init_power(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill init_power %s", 
        name_.c_str(), msg.id.c_str());
    init_power_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_init_power() {
    this->interrupt_init_power(init_power_status_.id);
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::safe_poweroff_response_callback(const quadruped_example_skillset_interfaces::msg::SkillSafePoweroffResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received safe_poweroff %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->safe_poweroff_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_safe_poweroff() {
    quadruped_example_skillset_interfaces::msg::SkillSafePoweroffRequest msg;
    
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill safe_poweroff %s", 
        name_.c_str(), msg.id.c_str());
    safe_poweroff_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_safe_poweroff(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill safe_poweroff %s", 
        name_.c_str(), msg.id.c_str());
    safe_poweroff_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_safe_poweroff() {
    this->interrupt_safe_poweroff(safe_poweroff_status_.id);
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::standup_response_callback(const quadruped_example_skillset_interfaces::msg::SkillStandupResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received standup %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->standup_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_standup() {
    quadruped_example_skillset_interfaces::msg::SkillStandupRequest msg;
    
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill standup %s", 
        name_.c_str(), msg.id.c_str());
    standup_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_standup(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill standup %s", 
        name_.c_str(), msg.id.c_str());
    standup_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_standup() {
    this->interrupt_standup(standup_status_.id);
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::sitdown_response_callback(const quadruped_example_skillset_interfaces::msg::SkillSitdownResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received sitdown %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->sitdown_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_sitdown() {
    quadruped_example_skillset_interfaces::msg::SkillSitdownRequest msg;
    
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill sitdown %s", 
        name_.c_str(), msg.id.c_str());
    sitdown_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_sitdown(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill sitdown %s", 
        name_.c_str(), msg.id.c_str());
    sitdown_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_sitdown() {
    this->interrupt_sitdown(sitdown_status_.id);
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::go_to_response_callback(const quadruped_example_skillset_interfaces::msg::SkillGoToResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received go_to %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->go_to_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_go_to() {
    quadruped_example_skillset_interfaces::msg::SkillGoToRequest msg;
    msg.input = go_to_input_;
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill go_to %s", 
        name_.c_str(), msg.id.c_str());
    go_to_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_go_to(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill go_to %s", 
        name_.c_str(), msg.id.c_str());
    go_to_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_go_to() {
    this->interrupt_go_to(go_to_status_.id);
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::recharge_response_callback(const quadruped_example_skillset_interfaces::msg::SkillRechargeResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received recharge %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->recharge_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_recharge() {
    quadruped_example_skillset_interfaces::msg::SkillRechargeRequest msg;
    
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill recharge %s", 
        name_.c_str(), msg.id.c_str());
    recharge_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_recharge(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill recharge %s", 
        name_.c_str(), msg.id.c_str());
    recharge_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_recharge() {
    this->interrupt_recharge(recharge_status_.id);
}

//-----------------------------------------------------------------------------

void QuadrupedExampleSkillsetClient::take_picture_response_callback(const quadruped_example_skillset_interfaces::msg::SkillTakePictureResponse::SharedPtr msg) {
    RCLCPP_INFO(this->node_->get_logger(), "[%s] received take_picture %s result %d %s", 
        name_.c_str(), msg->id.c_str(), msg->result, msg->name.c_str());
    this->take_picture_result_ = *msg;
    
}

std::string QuadrupedExampleSkillsetClient::start_take_picture() {
    quadruped_example_skillset_interfaces::msg::SkillTakePictureRequest msg;
    
    msg.id = generate_id();
    RCLCPP_INFO(this->node_->get_logger(), "[%s] start skill take_picture %s", 
        name_.c_str(), msg.id.c_str());
    take_picture_request_pub_->publish(msg);
    return msg.id;
}

void QuadrupedExampleSkillsetClient::interrupt_take_picture(std::string id) {
    quadruped_example_skillset_interfaces::msg::SkillInterrupt msg;
    msg.id = id;
    RCLCPP_INFO(this->node_->get_logger(), "[%s] interrupt skill take_picture %s", 
        name_.c_str(), msg.id.c_str());
    take_picture_interrupt_pub_->publish(msg);
}

void QuadrupedExampleSkillsetClient::interrupt_take_picture() {
    this->interrupt_take_picture(take_picture_status_.id);
}

