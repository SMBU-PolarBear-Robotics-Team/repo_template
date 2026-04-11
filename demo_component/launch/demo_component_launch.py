# Copyright 2026 Lihan Chen
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, GroupAction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PythonExpression
from launch_ros.actions import (
    ComposableNodeContainer,
    LoadComposableNodes,
    Node,
    SetParameter,
)
from launch_ros.descriptions import ComposableNode, ParameterFile


def generate_launch_description():
    package_share_directory = get_package_share_directory("demo_component")

    namespace = LaunchConfiguration("namespace")
    use_sim_time = LaunchConfiguration("use_sim_time")
    params_file = LaunchConfiguration("params_file")
    use_composition = LaunchConfiguration("use_composition")
    container_name = LaunchConfiguration("container_name")
    container_name_full = (namespace, "/", container_name)
    use_respawn = LaunchConfiguration("use_respawn")
    log_level = LaunchConfiguration("log_level")

    remappings = []

    configured_params = ParameterFile(params_file, allow_substs=True)

    declare_namespace_cmd = DeclareLaunchArgument(
        "namespace", default_value="", description="Top-level namespace"
    )
    declare_use_sim_time_cmd = DeclareLaunchArgument(
        "use_sim_time", default_value="false", description="Use simulation clock"
    )
    declare_params_file_cmd = DeclareLaunchArgument(
        "params_file",
        default_value=os.path.join(
            package_share_directory, "params", "demo_component.yaml"
        ),
        description="Full path to the ROS 2 parameters file",
    )
    declare_use_composition_cmd = DeclareLaunchArgument(
        "use_composition", default_value="False", description="Run as composable node"
    )
    declare_container_name_cmd = DeclareLaunchArgument(
        "container_name",
        default_value="demo_container",
        description="Composable container name",
    )
    declare_use_respawn_cmd = DeclareLaunchArgument(
        "use_respawn", default_value="False", description="Respawn node if it crashes"
    )
    declare_log_level_cmd = DeclareLaunchArgument(
        "log_level", default_value="info", description="Log level"
    )

    load_node = GroupAction(
        condition=IfCondition(PythonExpression(["not ", use_composition])),
        actions=[
            SetParameter("use_sim_time", use_sim_time),
            Node(
                package="demo_component",
                executable="demo_component_node",
                name="demo_component",
                output="screen",
                respawn=use_respawn,
                respawn_delay=2.0,
                parameters=[configured_params],
                arguments=["--ros-args", "--log-level", log_level],
                remappings=remappings,
            ),
        ],
    )

    load_composable_nodes = GroupAction(
        condition=IfCondition(use_composition),
        actions=[
            SetParameter("use_sim_time", use_sim_time),
            ComposableNodeContainer(
                name=container_name,
                namespace=namespace,
                package="rclcpp_components",
                executable="component_container_mt",
                composable_node_descriptions=[],
                output="screen",
            ),
            LoadComposableNodes(
                target_container=container_name_full,
                composable_node_descriptions=[
                    ComposableNode(
                        package="demo_component",
                        plugin="demo_component::DemoComponentNode",
                        name="demo_component",
                        parameters=[configured_params],
                        remappings=remappings,
                    ),
                ],
            ),
        ],
    )

    ld = LaunchDescription()
    ld.add_action(declare_namespace_cmd)
    ld.add_action(declare_use_sim_time_cmd)
    ld.add_action(declare_params_file_cmd)
    ld.add_action(declare_use_composition_cmd)
    ld.add_action(declare_container_name_cmd)
    ld.add_action(declare_use_respawn_cmd)
    ld.add_action(declare_log_level_cmd)
    ld.add_action(load_node)
    ld.add_action(load_composable_nodes)

    return ld
