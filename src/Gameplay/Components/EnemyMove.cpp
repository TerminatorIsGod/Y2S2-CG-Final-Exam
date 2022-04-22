#include "Gameplay/Components/EnemyMove.h"
#include <GLFW/glfw3.h>
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Utils/ImGuiHelper.h"
#include "Gameplay/InputEngine.h"
#include <Application/Application.h>

void EnemyMove::Awake()
{

}

void EnemyMove::RenderImGui() {
	LABEL_LEFT(ImGui::DragFloat, "Speed", &_speed, 1.0f);
}

nlohmann::json EnemyMove::ToJson() const {
	return {
		{ "speed", _speed }
	};
}

EnemyMove::EnemyMove() :
	IComponent(),
	_speed(1.0f)
{ }

EnemyMove::~EnemyMove() = default;

EnemyMove::Sptr EnemyMove::FromJson(const nlohmann::json& blob) {
	EnemyMove::Sptr result = std::make_shared<EnemyMove>();
	result->_speed = blob["speed"];
	return result;
}

void EnemyMove::Update(float deltaTime) {
	auto _body = GetGameObject()->GetPosition();
	auto _player = GetGameObject()->GetScene()->MainCamera;

	glm::vec3 dist = _player->GetGameObject()->GetPosition() - _body;
	float length = dist.length();
	glm::vec3 normalized = glm::vec3(dist.x/length, dist.y/length, dist.z/length);
	normalized.z = 0;
	GetGameObject()->SetPostion(_body + (normalized * _speed * deltaTime));

	//_body->ApplyForce(normalized * _speed);
}

void EnemyMove::OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) {
	std::cout << "\n\nYou died... well thats too bad\n\n";
	Application& app = Application::Get();
	app.RestartScene("scene.json");
}

