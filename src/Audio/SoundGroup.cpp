#include "SoundGroup.hpp"

SoundGroup::SoundGroup()
	: _bandNumber(0), _meanMagnitude(0), _meanDelta(0), _meanIndex(0), _lastIndexAdded(0)
{}

SoundGroup::SoundGroup(int newBandNumber, float newMeanMagnitude, float newMeanDelta,int newMeanIndex)
	: _bandNumber(newBandNumber), _meanMagnitude(newMeanMagnitude), _meanDelta(newMeanDelta), _meanIndex(newMeanIndex), _lastIndexAdded(0)
{}

bool SoundGroup::canJoinGroup(const float& magnitude, const float& delta, const int& index) const
{
	const float magnitudeCoeff = calculateMagnitudeCoeff(magnitude);
	const float distanceCoeff = calculateDistanceCoeff(index);

	if (delta < _meanDelta - _meanDelta * (magnitudeCoeff * distanceCoeff))
		return false;
	if (delta > _meanDelta + _meanDelta * (magnitudeCoeff * distanceCoeff))
		return false;

	return true;
}

float SoundGroup::calculateMagnitudeCoeff(const float& magnitude) const
{
	if (_meanMagnitude >= 0.4)
		return 0.66;

	return .00063709 * (_meanMagnitude*_meanMagnitude) - .0608768 * _meanMagnitude + 2.10166;
}

float SoundGroup::calculateDistanceCoeff(const int& index) const
{
	const int distance = abs(index - _lastIndexAdded);

	// Based on a max distance of 64 (which is AudioAnalyze output array size)
	const double slope = -0.007936; // distance 1 = 1 | distance 63 = 0.5

	return slope * distance + 1.0;
}

void SoundGroup::addToGroup(const float &magnitude, const float &delta, const int &index)
{
	float temp;

	// Updating groups's mean magnitude
	temp = _bandNumber * _meanMagnitude;
	_meanMagnitude = (temp + magnitude) / (_bandNumber + 1);

	// Updating group's mean index
	temp = _bandNumber * _meanIndex;
	_meanIndex = (double)(temp + index) / (double)(_bandNumber + 1);

	// Updating group's mean Delta
	temp = _bandNumber * _meanDelta;
	_meanDelta = (temp + delta) / (_bandNumber + 1);

	_bandNumber += 1;
	_lastIndexAdded = index;
}

int SoundGroup::getBandNumber() const { return _bandNumber; }
float SoundGroup::getMeanMagnitude() const { return _meanMagnitude; }
float SoundGroup::getMeanDelta() const { return _meanDelta; }
int SoundGroup::getMeanIndex() const { return _meanIndex; }

void SoundGroup::setBandNumber(const int& newBandNumber) { _bandNumber = newBandNumber; }
void SoundGroup::setMeanMagnitude(const float& newMeanMagnitude) { _meanMagnitude = newMeanMagnitude; }
void SoundGroup::setMeanDelta(const float& newMeanDelta) { _meanDelta = newMeanDelta; }
void SoundGroup::setMeanIndex(const int& newMeanIndex) { _meanIndex = newMeanIndex; }
