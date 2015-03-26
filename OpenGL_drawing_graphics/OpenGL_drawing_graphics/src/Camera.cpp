#include "Camera.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

Camera::Camera() : Camera(glm::mat4(1))
{

}

Camera::Camera(glm::vec3 _position) : Camera()
{
	worldTransform[3] = glm::vec4(_position, 1);
	perspectiveSet = false;
}

Camera::Camera(glm::mat4 _transform) : worldTransform(_transform)
{
	UpdateProjectionViewTransform();
}

void Camera::UpdateProjectionViewTransform()
{
	//invert the matrix to get the camera transform
	viewTransform = inverse(worldTransform);

	//multiply the projection and view transform matrices 
	//apply them to the projectionview matrix
	projectionViewTransform = projectionTransform * viewTransform;
}

void Camera::SetWorldTransform(glm::mat4 _transform)
{
	worldTransform = _transform;
	UpdateProjectionViewTransform();
}

const glm::mat4 Camera::GetWorldTransform()
{
	return worldTransform;
}

const glm::mat4 Camera::GetView()
{
	return viewTransform;
}

const glm::mat4 Camera::GetProjection()
{
	return projectionTransform;
}

const glm::mat4 Camera::GetProjectionView()
{
	return projectionViewTransform;
}

void Camera::SetPosition(glm::vec3 _position)
{
	//accessing the 4th row of the matrix where positional data is stored, W defaults to 1
	worldTransform[3] = glm::vec4(_position, 1);
	UpdateProjectionViewTransform();
}

glm::vec3 Camera::GetPosition()
{
	//return the 4th part of the transform matrix then swizzle the XYZ component
	return worldTransform[3].xyz();
}

void Camera::SetUpPerspective(float _fov, float _aspectRatio, float _near, float _far)
{
	projectionTransform = glm::perspective(_fov, _aspectRatio, _near, _far);
	perspectiveSet = true;
	UpdateProjectionViewTransform();
}

const bool Camera::GetPerspectiveSet()
{
	return perspectiveSet;
}

void Camera::LookAt(glm::vec3 _position, glm::vec3 _lookAt, glm::vec3 _up)
{
	worldTransform = glm::inverse(lookAt(_position, _lookAt, _up));
	UpdateProjectionViewTransform();
}

void Camera::LookAt(glm::vec3 _lookAt, glm::vec3 _up)
{
	//set location to the position component of the matrix
	glm::vec4 location = worldTransform[3];
	worldTransform = glm::inverse(lookAt(location.xyz(), _lookAt, _up));
	UpdateProjectionViewTransform();
}