#pragma once
#include "IComponent.h"
#include "Gameplay/Physics/RigidBody.h"

/// <summary>
/// A simple behaviour that applies an impulse along the Z axis to the 
/// rigidbody of the parent when the space key is pressed
/// </summary>
class EnemyMove : public Gameplay::IComponent {
public:
	typedef std::shared_ptr<EnemyMove> Sptr;

	std::weak_ptr<Gameplay::IComponent> Panel;

	EnemyMove();
	virtual ~EnemyMove();

	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void OnTriggerVolumeEntered(const std::shared_ptr<Gameplay::Physics::RigidBody>& body) override;

public:
	virtual void RenderImGui() override;
	MAKE_TYPENAME(EnemyMove);
	virtual nlohmann::json ToJson() const override;
	static EnemyMove::Sptr FromJson(const nlohmann::json& blob);

protected:
	float _impulse;
	float _speed;

	bool _isPressed = false;
	Gameplay::Physics::RigidBody::Sptr _body;
};