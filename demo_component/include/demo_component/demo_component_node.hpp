// Copyright 2026 Lihan Chen
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DEMO_COMPONENT__DEMO_COMPONENT_NODE_HPP_
#define DEMO_COMPONENT__DEMO_COMPONENT_NODE_HPP_

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

#include "demo_interfaces/action/count_steps.hpp"
#include "demo_interfaces/msg/demo_state.hpp"
#include "demo_interfaces/srv/set_mode.hpp"
#include "rclcpp_action/rclcpp_action.hpp"

namespace demo_component
{

class DemoComponentNode : public rclcpp::Node
{
public:
  explicit DemoComponentNode(const rclcpp::NodeOptions & options);

private:
  using CountSteps = demo_interfaces::action::CountSteps;
  using GoalHandleCountSteps = rclcpp_action::ServerGoalHandle<CountSteps>;
  using SetMode = demo_interfaces::srv::SetMode;

  void publishState();

  void handleSetMode(
    const std::shared_ptr<SetMode::Request> request, std::shared_ptr<SetMode::Response> response);

  rclcpp_action::GoalResponse handleCountStepsGoal(
    const rclcpp_action::GoalUUID & goal_uuid, std::shared_ptr<const CountSteps::Goal> goal);

  rclcpp_action::CancelResponse handleCountStepsCancel(
    const std::shared_ptr<GoalHandleCountSteps> goal_handle);

  void handleCountStepsAccepted(const std::shared_ptr<GoalHandleCountSteps> goal_handle);

  void executeCountSteps(const std::shared_ptr<GoalHandleCountSteps> goal_handle);

  rclcpp::Publisher<demo_interfaces::msg::DemoState>::SharedPtr state_publisher_;
  rclcpp::Service<SetMode>::SharedPtr set_mode_service_;
  rclcpp_action::Server<CountSteps>::SharedPtr count_steps_server_;

  std::string mode_{"idle"};
  std::uint32_t count_{0};
  std::mutex state_mutex_;
};

}  // namespace demo_component

#endif  // DEMO_COMPONENT__DEMO_COMPONENT_NODE_HPP_
