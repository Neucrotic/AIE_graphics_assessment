#include <iostream>
#include "MobileCamera.h"
#include <glm/ext.hpp>

using namespace std;

MobileCamera::MobileCamera() : MobileCamera(10.0f)
{
	viewButtonClicked = false;
}

MobileCamera::MobileCamera(float _moveSpeed)
{
	moveSpeed = _moveSpeed;
	viewButtonClicked = false;
}

void MobileCamera::Update(double _dt)
{
	//functions will not work if the window has not been set
	HandleKeyboardInput(_dt);
	HandleMouseInput(_dt);
	UpdateProjectionViewTransform();
}

void MobileCamera::SetMoveSpeed(float _moveSpeed)
{
	moveSpeed = _moveSpeed;
}

const float MobileCamera::GetMoveSpeed()
{
	return moveSpeed;
}

void MobileCamera::SetInputWindow(GLFWwindow* _window)
{
	window = _window;
}

GLFWwindow* MobileCamera::GetInputWindow()
{
	return window;
}

void MobileCamera::HandleKeyboardInput(double _dt)
{
	//get the cameras up, forward and right
	glm::vec3 right = worldTransform[0].xyz;
	glm::vec3 up = worldTransform[1].xyz;
	glm::vec3 forward = worldTransform[2].xyz;
	//initialising move direction
	glm::vec3 moveDirection(0.0f);

	//check keypresses and move camera based on controls
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDirection -= forward * _dt;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDirection += forward * _dt;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDirection -= right * _dt;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDirection += right * _dt;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		moveDirection += glm::vec3(0.0f, 1.0f, 0.0f)  * _dt;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDirection -= glm::vec3(0.0f, 1.0f, 0.0f) * _dt;
	}

	//normalising direction when necessary
	float dirLength = glm::length(moveDirection);
	if (dirLength > 0.01f)
	{
		moveDirection = ((float)_dt * moveSpeed) * glm::normalize(moveDirection);
		SetPosition(GetPosition() + moveDirection);
	}
}

void MobileCamera::HandleMouseInput(double _dt)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (viewButtonClicked == false)
		{
			int width;
			int height;

			glfwGetFramebufferSize(window, &width, &height);

			cursorX = width / 2.0;
			cursorY = height / 2.0;
			glfwSetCursorPos(window, cursorX, cursorY);
			viewButtonClicked = true;
		}
		else
		{
			double mouseX;
			double mouseY;

			glfwGetCursorPos(window, &mouseX, &mouseY);

			double xOffset = mouseX - cursorX;
			double yOffset = mouseY - cursorY;

			CalcuateRotation(glm::vec2(xOffset, yOffset), _dt);

			//debug message output
			cout << xOffset << " " << yOffset << endl;
		}
	}
	else
	{
		viewButtonClicked = false;
	}
}

void MobileCamera::CalcuateRotation(glm::vec2 _offset, double _dt)
{
	glm::vec3 up = glm::vec3(0, 1, 0);
	glm::vec3 right = glm::vec3(1, 0, 0);

	glm::normalize(up);
	glm::normalize(right);

	glm::mat4 xRotation = glm::rotate(_offset.x * (float)_dt * 0.01f, up);
	glm::mat4 yRotation = glm::rotate(_offset.y * (float)_dt * 0.01f, right);

	if (glm::length(_offset) > 0.001f)
	{
		worldTransform = worldTransform * xRotation;
		worldTransform = worldTransform * yRotation;
	}

	//restrict weird vectors
	right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), glm::vec3(worldTransform[2])));
	up = glm::normalize(glm::cross(glm::vec3(worldTransform[2]), right));

	worldTransform[2] = glm::vec4(glm::normalize(glm::vec3(worldTransform[2])), 0);
	glm::normalize(up);
	glm::normalize(right);

	worldTransform[0] = glm::vec4(right, 0);
	worldTransform[1] = glm::vec4(up, 0);

	UpdateProjectionViewTransform();
}