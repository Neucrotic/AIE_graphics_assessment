#pragma once
#include "Camera.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class MobileCamera : public Camera
{
protected:
	
	GLFWwindow* window;
	float moveSpeed;

	//flag to see if the viewing button is clicked
	bool viewButtonClicked;

	//cursor co-ordinates
	double cursorX;
	double cursorY;

	//to be replaced by input manager functionality
	void HandleKeyboardInput(double _dt);
	void HandleMouseInput(double _dt);

	void CalcuateRotation(glm::vec2 _offset, double _dt);
public:

	MobileCamera();
	MobileCamera(float _moveSpeed);
	~MobileCamera() {};

	void Update(double _dt);

	void SetMoveSpeed(float _speed);
	const float GetMoveSpeed();

	void SetInputWindow(GLFWwindow* _window);
	GLFWwindow* GetInputWindow();
};