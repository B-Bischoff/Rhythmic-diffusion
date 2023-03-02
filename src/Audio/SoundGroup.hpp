#pragma once

#include "../../inc/include.hpp"

class SoundGroup {
private:
	int _bandNumber;
	float _meanMagnitude;
	float _meanDelta;
	int _meanIndex;
	int _lastIndexAdded;

	float calculateMagnitudeCoeff(const float& magnitude) const;
	float calculateDistanceCoeff(const int& index) const;

public:
	SoundGroup();
	SoundGroup(int newBandNumber, float newMeanMagnitude, float newMeanDelta,int newMeanIndex);

	bool canJoinGroup(const float& magnitude, const float& delta, const int& index) const;
	void addToGroup(const float& magnitude, const float& delta, const int& index);

	int getBandNumber() const;
	float getMeanMagnitude() const;
	float getMeanDelta() const;
	int getMeanIndex() const;

	void setBandNumber(const int& newBandNumber);
	void setMeanMagnitude(const float& newMeanMagnitude);
	void setMeanDelta(const float& newMeanDelta);
	void setMeanIndex(const int& newMeanIndex);
};
