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

#include "demo_component/demo_component_node.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace demo_component
{

DemoComponentNode::DemoComponentNode(const rclcpp::NodeOptions & options)
: Node("demo_component", options)
{
  this->declare_parameter<std::string>("state_topic", "demo_state");

  std::string state_topic;
  this->get_parameter("state_topic", state_topic);

  state_publisher_ = this->create_publisher<demo_interfaces::msg::DemoState>(state_topic, 10);
  set_mode_service_ = this->create_service<SetMode>(
    "set_mode",
    std::bind(
      &DemoComponentNode::handleSetMode, this, std::placeholders::_1, std::placeholders::_2));

  count_steps_server_ = rclcpp_action::create_server<CountSteps>(
    this, "count_steps",
    std::bind(
      &DemoComponentNode::handleCountStepsGoal, this, std::placeholders::_1, std::placeholders::_2),
    std::bind(&DemoComponentNode::handleCountStepsCancel, this, std::placeholders::_1),
    std::bind(&DemoComponentNode::handleCountStepsAccepted, this, std::placeholders::_1));

  publishState();
}

void DemoComponentNode::publishState()
{
  demo_interfaces::msg::DemoState msg;
  {
    std::lock_guard<std::mutex> lock(state_mutex_);
    msg.mode = mode_;
    msg.count = count_;
  }
  state_publisher_->publish(msg);
}

void DemoComponentNode::handleSetMode(
  const std::shared_ptr<SetMode::Request> request, std::shared_ptr<SetMode::Response> response)
{
  if (request->mode.empty()) {
    response->success = false;
    response->message = "mode must not be empty";
    return;
  }

  {
    std::lock_guard<std::mutex> lock(state_mutex_);
    mode_ = request->mode;
  }

  response->success = true;
  response->message = "mode set to " + mode_;
  publishState();
}

rclcpp_action::GoalResponse DemoComponentNode::handleCountStepsGoal(
  const rclcpp_action::GoalUUID & goal_uuid, std::shared_ptr<const CountSteps::Goal> goal)
{
  (void)goal_uuid;

  if (goal->target < 0) {
    return rclcpp_action::GoalResponse::REJECT;
  }

  return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
}

rclcpp_action::CancelResponse DemoComponentNode::handleCountStepsCancel(
  const std::shared_ptr<GoalHandleCountSteps> goal_handle)
{
  (void)goal_handle;

  return rclcpp_action::CancelResponse::ACCEPT;
}

void DemoComponentNode::handleCountStepsAccepted(
  const std::shared_ptr<GoalHandleCountSteps> goal_handle)
{
  std::thread{std::bind(&DemoComponentNode::executeCountSteps, this, goal_handle)}.detach();
}

void DemoComponentNode::executeCountSteps(const std::shared_ptr<GoalHandleCountSteps> goal_handle)
{
  const auto goal = goal_handle->get_goal();
  const std::uint32_t target = static_cast<std::uint32_t>(std::max(goal->target, 0));

  auto feedback = std::make_shared<CountSteps::Feedback>();
  auto result = std::make_shared<CountSteps::Result>();

  while (rclcpp::ok()) {
    std::uint32_t current_count = 0;
    bool reached_target = false;

    {
      std::lock_guard<std::mutex> lock(state_mutex_);
      if (count_ >= target) {
        reached_target = true;
      } else {
        ++count_;
        current_count = count_;
      }
    }

    if (reached_target) {
      result->finished = true;
      goal_handle->succeed(result);
      publishState();
      return;
    }

    if (goal_handle->is_canceling()) {
      result->finished = false;
      goal_handle->canceled(result);
      publishState();
      return;
    }

    feedback->current = static_cast<int32_t>(current_count);
    goal_handle->publish_feedback(feedback);
    publishState();
  }

  result->finished = false;
  goal_handle->abort(result);
}

}  // namespace demo_component

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(demo_component::DemoComponentNode)
