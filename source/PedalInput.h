#pragma once

#include <cmgInput/cmg_input.h>

class PedalInput
{
public:
	PedalInput() {}
	PedalInput(const String& name, float minAmount, float maxAmount, float pressThreshold, float deadZone = 0.0f);

	bool IsDown() const;
	bool IsPressed() const;
	bool IsReleased() const;
	float GetAmount() const;
	const String& GetName() const;

	void Update(float amount);


private:
	String m_name;
	float m_rawAmount = 0.0f;
	float m_amount = 0.0f;
	float m_amountPrev = 0.0f;
	float m_minAmount = 0.0f;
	float m_maxAmount = 1.0f;
	float m_deadZone = 0.0f;
	float m_pressThreshold = 0.1f;
};
