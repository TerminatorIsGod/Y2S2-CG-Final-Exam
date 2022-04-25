#include "Gameplay/Components/SimpleCameraControl.h"
#include <GLFW/glfw3.h>
#define  GLM_SWIZZLE
#include <GLM/gtc/quaternion.hpp>

#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/ImGuiHelper.h"
#include "Gameplay/InputEngine.h"
#include "Application/Application.h"
#include "Gameplay/Components/GUI/RectTransform.h"
#include <Utils/GlmBulletConversions.h>

SimpleCameraControl::SimpleCameraControl() :
	IComponent(),
	_mouseSensitivity({ 0.5f, 0.3f }),
	_moveSpeeds(glm::vec3(200.0f)),
	_shiftMultipler(2.0f),
	_currentRot(glm::vec2(0.0f)),
	_isMousePressed(true)
{ }

SimpleCameraControl::~SimpleCameraControl() = default;

void SimpleCameraControl::Awake() {
	_scene = GetGameObject()->GetScene();
	Application& app = Application::Get();
	_window = app.GetWindow();
}

void SimpleCameraControl::Movement(float deltaTime)
{
	auto _body = GetComponent<Gameplay::Physics::RigidBody>();

	if (glfwGetKey(_window, GLFW_KEY_M) && _allowMouse == false) {
		_isMousePressed = !_isMousePressed;
		_allowMouse = true;

		std::cout << "Changing mouse thing\n";
	}
	else if (!glfwGetKey(_window, GLFW_KEY_M)) {
		_allowMouse = false;
	}

	if (GetGameObject()->GetPosition().z <= -50) {
		Application& app = Application::Get();
		app.RestartScene("scene");
	}

	if (_isMousePressed) {
		glm::dvec2 currentMousePos;
		glfwGetCursorPos(_window, &currentMousePos.x, &currentMousePos.y);

		int wsizex, wsizey;

		glfwGetWindowSize(_window, &wsizex, &wsizey);

		float centerx = (wsizex / 2);
		float centery = (wsizey / 2);

		float xoffset = centerx - currentMousePos.x;
		float yoffset = centery - currentMousePos.y;


		glfwSetCursorPos(_window, centerx, centery);


		_currentRot.x += static_cast<float>(xoffset) * _mouseSensitivity.x;  //_currentRot.x += static_cast<float>(currentMousePos.x - _prevMousePos.x) * _mouseSensitivity.x;
		_currentRot.y += static_cast<float>(yoffset) * _mouseSensitivity.y;
		//std::cout << "\nY Rot: " << _currentRot.y;
		if (_currentRot.y > 172)
			_currentRot.y = 172;
		else if (_currentRot.y < 4.5)
			_currentRot.y = 4.5;

		glm::quat rotX = glm::angleAxis(glm::radians(_currentRot.x), glm::vec3(0, 0, 1));
		glm::quat rotY = glm::angleAxis(glm::radians(_currentRot.y), glm::vec3(1, 0, 0));
		currentRot = rotX * rotY;

		GetGameObject()->SetRotation(currentRot);

		_prevMousePos = currentMousePos;

		glm::vec3 input = glm::vec3(0.0f);
		if (glfwGetKey(_window, GLFW_KEY_W)) {
			input.z = -_moveSpeeds.x;
		}
		if (glfwGetKey(_window, GLFW_KEY_S)) {
			input.z = _moveSpeeds.x;
		}
		if (glfwGetKey(_window, GLFW_KEY_A)) {
			input.x = -_moveSpeeds.y;
		}
		if (glfwGetKey(_window, GLFW_KEY_D)) {
			input.x = _moveSpeeds.y;
		}

		glm::vec3 worldMovement = currentRot * glm::vec4(input, 1.0f);


		if (_body == nullptr) {
			GetGameObject()->SetPostion(GetGameObject()->GetPosition() + worldMovement);
			return;
		}


		_body->SetAngularFactor(glm::vec3(0, 0, 0));
		glm::vec3 physicsMovement = worldMovement;
		physicsMovement.z = 0.0f;

		btCollisionWorld::ClosestRayResultCallback hit(ToBt(GetGameObject()->GetPosition()), ToBt( GetGameObject()->GetPosition() + glm::vec3(0, 0, -1.0f) ));
		_scene->GetPhysicsWorld()->rayTest(ToBt(GetGameObject()->GetPosition()), ToBt(GetGameObject()->GetPosition() + glm::vec3(0, 0, -1.0f)), hit);

		if (hit.hasHit()) {
			if (glfwGetKey(_window, GLFW_KEY_SPACE)) {
				_jumpTimer = 0.0f;
			}
		}

		if (_jumpTimer <= 0.5f) {
			_jumpTimer += deltaTime;
			physicsMovement.z = _moveSpeeds.z;
		}

		physicsMovement.z += -98.1f;
		_body->SetLinearVelocity(glm::vec3(physicsMovement * deltaTime));
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else {
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void SimpleCameraControl::Update(float deltaTime)
{
	Movement(deltaTime);
}

void SimpleCameraControl::RenderImGui()
{
	LABEL_LEFT(ImGui::DragFloat2, "Mouse Sensitivity", &_mouseSensitivity.x, 0.01f);
	LABEL_LEFT(ImGui::DragFloat3, "Move Speed       ", &_moveSpeeds.x, 0.01f, 0.01f);
	LABEL_LEFT(ImGui::DragFloat , "Shift Multiplier ", &_shiftMultipler, 0.01f, 1.0f);
}

nlohmann::json SimpleCameraControl::ToJson() const {
	return {
		{ "mouse_sensitivity", _mouseSensitivity },
		{ "move_speed", _moveSpeeds },
		{ "shift_mult", _shiftMultipler }
	};
}

SimpleCameraControl::Sptr SimpleCameraControl::FromJson(const nlohmann::json& blob) {
	SimpleCameraControl::Sptr result = std::make_shared<SimpleCameraControl>();
	result->_mouseSensitivity = JsonGet(blob, "mouse_sensitivity", result->_mouseSensitivity);
	result->_moveSpeeds       = JsonGet(blob, "move_speed", result->_moveSpeeds);
	result->_shiftMultipler   = JsonGet(blob, "shift_mult", 2.0f);
	return result;
}
