#pragma once

#include <glm/glm.hpp>

class Camera
{
protected:
	
	bool perspectiveSet;

	//the camera's world origin
	glm::mat4 worldTransform;
	//transforms the world, making the camera the origin
	glm::mat4 viewTransform;
	//position of the world on the screen
	glm::mat4 projectionTransform;

	//created by multiplying the projection, view and world transform
	//the final transform matrix of the objects, dictating their screen coordinates
	glm::mat4 projectionViewTransform;

	void UpdateProjectionViewTransform();
public:

	Camera();
	Camera(glm::vec3 _position);
	Camera(glm::mat4 _transform);

	virtual void Update(double _dt) = 0;
	
	//get'ers and set'ers
	//transforms
	void SetWorldTransform(glm::mat4 _transform);
	const glm::mat4 GetWorldTransform();
	//views and projections
	const glm::mat4 GetView();
	const glm::mat4 GetProjection();
	const glm::mat4 GetProjectionView();
	//positions
	void SetPosition(glm::vec3 _position);
	glm::vec3 GetPosition();
	//perspectives
	void SetUpPerspective(float _fov, float _aspectRatio, float _near = 0.1f, float _far = 1000.0f);
	//check if perspectives are set
	const bool GetPerspectiveSet();

	//setting FOV targets
	void LookAt(glm::vec3 _position, glm::vec3 _lookAt, glm::vec3 _up);
	void LookAt(glm::vec3 _lookAt, glm::vec3 _up);
};