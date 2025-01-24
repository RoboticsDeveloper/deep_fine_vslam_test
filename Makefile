.PHONY: default up enter down build xhost-enable xhost-disable

default: up enter

up:
	IMAGE_NAME=glim_extension_ros2:humble \
	CONTAINER_NAME=slam_container \
	docker compose up -d

enter:
	docker exec -it slam_container bash

down:
	IMAGE_NAME=glim_extension_ros2:humble \
	CONTAINER_NAME=slam_container \
	docker compose down

build:
	docker build -f docker/Dockerfile.ros2 -t glim_extension_ros2:humble .

xhost-enable:
	xhost +local:root

xhost-disable:
	xhost -local:root
