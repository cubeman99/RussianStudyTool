#include "PedalInput.h"

PedalInput::PedalInput(const String & name, float minAmount, float maxAmount,
	float pressThreshold, float deadZone) :
	m_name(name),
	m_minAmount(minAmount),
	m_maxAmount(maxAmount),
	m_pressThreshold(pressThreshold),
	m_deadZone(deadZone)
{
}

bool PedalInput::IsDown() const
{
	return (m_amount >= m_pressThreshold);
}

bool PedalInput::IsPressed() const
{
	return (m_amount >= m_pressThreshold &&
		m_amountPrev < m_pressThreshold);
}

bool PedalInput::IsReleased() const
{
	return (m_amount < m_pressThreshold &&
		m_amountPrev >= m_pressThreshold);
}

float PedalInput::GetAmount() const
{
	return m_amount;
}

const String& PedalInput::GetName() const
{
	return m_name;
}

void PedalInput::Update(float amount)
{
	m_rawAmount = amount;
	m_amountPrev = m_amount;
	m_amount = (amount - m_minAmount) / (m_maxAmount - m_minAmount);
	m_amount = Math::Clamp(m_amount, 0.0f, 1.0f);
	if (m_amount < m_deadZone)
		m_amount = 0.0f;
}

