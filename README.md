# repo_template

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![Build](https://github.com/LihanChen2004/nav2_neupan_controller/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/LihanChen2004/nav2_neupan_controller/actions/workflows/build_and_test.yml)

## 1. Overview

A minimal ROS 2 workspace template with two related packages:

- `demo_interfaces`: custom message, service, and action definitions
- `demo_component`: a composable C++ node that uses the generated interfaces

## 2. Quick Start

### 2.1 Setup Environment

- [Ubuntu 24.04](https://releases.ubuntu.com/noble/)
- [ROS2 humble](https://docs.ros.org/en/humble/index.html)

### 2.2 Create Workspace

```bash
mkdir -p ~/ros_ws
cd ~/ros_ws
```

```bash
git clone https://github.com/SMBU-PolarBear-Robotics-Team/repo_template.git src/repo_template
```

## 2.3 Build and Test

Install dependencies

```bash
rosdep install -r --from-paths src --ignore-src --rosdistro $ROS_DISTRO -y
```

Build

```bash
colcon build \
  --merge-install \
  --symlink-install \
  --cmake-args "-DCMAKE_BUILD_TYPE=RelWithDebInfo" \
  --parallel-workers 10 \
  --packages-up-to \
  repo_template
```

Test (Optional)

```bash
colcon test \
  --merge-install \
  --parallel-workers 10 \
  --packages-up-to \
  repo_template

colcon test-result --all --verbose
```

### 2.4 Running

```bash
ros2 launch demo_component demo_component_launch.py
```

Talk to the demo node

Set the mode with the service:

```bash
ros2 service call /set_mode demo_interfaces/srv/SetMode "{mode: 'running'}"
```

Send a counting goal with the action:

```bash
ros2 action send_goal /count_steps demo_interfaces/action/CountSteps "{target: 5}"
```

Watch the published state:

```bash
ros2 topic echo /demo_state
```
