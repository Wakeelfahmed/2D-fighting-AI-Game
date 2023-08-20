#include "HealthCondition.h"

HealthCondition::HealthCondition(const bool okHealth)
{
	SetOkHealth(okHealth);
	m_name = "Health Condition";
}

HealthCondition::~HealthCondition() = default;

void HealthCondition::SetOkHealth(const bool state)
{
	m_okHealth = state;
}

bool HealthCondition::Condition()
{
	return m_okHealth;
}
