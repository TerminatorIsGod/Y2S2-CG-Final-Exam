#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Scene.h"

struct GLFWwindow;

/// <summary>
/// A simple behaviour that allows movement of a gameobject with WASD, mouse,
/// and ctrl + space
/// </summary>
class SimpleCameraControl : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<SimpleCameraControl> Sptr;

	SimpleCameraControl();
	virtual ~SimpleCameraControl();

	void Movement(float deltaTime);
	virtual void Update(float deltaTime) override;
	virtual void Awake() override;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(SimpleCameraControl);
	virtual nlohmann::json ToJson() const override;
	static SimpleCameraControl::Sptr FromJson(const nlohmann::json& blob);

	glm::vec3 viewDir;
	glm::quat currentRot;

protected:
	float _shiftMultipler;
	glm::vec2 _mouseSensitivity;
	glm::vec3 _moveSpeeds;
	glm::dvec2 _prevMousePos;
	glm::vec2 _currentRot;

	glm::vec2 centerPos;
	int windx, windy;

	bool _allowMouse = true;
	bool _isMousePressed = true;

	float _jumpTimer = 2.0f;

	GLFWwindow* _window;
	Gameplay::Scene* _scene;
};