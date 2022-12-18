#pragma once

struct InputParameterSettings {
	virtual ~InputParameterSettings() {};
};

struct NumberInput : public InputParameterSettings {
	~NumberInput() {}
	float value;
};
