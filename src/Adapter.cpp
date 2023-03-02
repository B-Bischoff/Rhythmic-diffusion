#include "Adapter.hpp"
#include "vendor/imgui/imgui.h"

Adapter::Adapter(ReactionDiffusionSimulator& RDSimulator, AudioAnalyzer& audioAnalyzer)
	: _RDSimulator(RDSimulator), _audioAnalyzer(audioAnalyzer)
{
}

void Adapter::update()
{
	normalizeGroupsOutputs();
	modifyReactionDiffusion();
}

void Adapter::normalizeGroupsOutputs()
{
	std::vector<SoundGroup>& groups = _audioAnalyzer.getGroups();

	// Reset previous ratios
	_bassRatio = 0;
	_snareRatio = 0;
	_leadRatio = 0;

	float bassMagnitude = 0;
	float bassDelta = 0;
	float snareMagnitude = 0;
	float leadMagnitude = 0;

	for (int i = 0; i < (int)groups.size(); i++)
	{
		SoundGroup& group = groups[i];
		if (group.getMeanDelta() < 0)
			continue;

		findBassRatioFromGroup(group, bassMagnitude, bassDelta);
		findSnareRatioFromGroup(group, snareMagnitude);
		findLeadRatioFromGroup(group);
	}
}

void Adapter::findBassRatioFromGroup(SoundGroup& group, float& bassMagnitude, float& bassDelta)
{
	const float minBassMagnitude = 50;
	const float maxBassMagnitude = 150;

	if (group.getMeanIndex() < 2 && group.getMeanMagnitude() > minBassMagnitude)
	{
		if (group.getMeanDelta() > bassDelta)
		{
			bassDelta = group.getMeanDelta();
			bassMagnitude = group.getMeanMagnitude();

			_bassRatio = bassMagnitude / (maxBassMagnitude - minBassMagnitude);
			if (_bassRatio > 1.0)
				_bassRatio = 1.0;
		}
	}
}

void Adapter::findSnareRatioFromGroup(SoundGroup& group, float& snareMagnitude)
{
	const float maxSnareMagnitude = 10;
	const float minSnareMagnitude = 2;

	if (group.getBandNumber() > 6 && group.getMeanIndex() > 20 && group.getMeanMagnitude() > minSnareMagnitude)
	{
		if (group.getMeanMagnitude() > snareMagnitude)
			snareMagnitude = group.getMeanMagnitude();

		_snareRatio = snareMagnitude / (maxSnareMagnitude - minSnareMagnitude);
		if (_snareRatio > 1.0)
			_snareRatio = 1.0;
	}
}

void Adapter::findLeadRatioFromGroup(SoundGroup& group)
{
	const float minMagnitude = 10;
	const float minDelta = 3.5;
	const float maxDelta = 10.0;

	if (group.getMeanIndex() < 5 || group.getMeanIndex() > 35) // Exclude bass and snare
		return;

	if ((group.getMeanDelta() > 6 && group.getMeanMagnitude() > 30) || group.getMeanMagnitude() > 50) // Accept high delta / magnitude groups
	{
		_leadRatio = 1.0;
		return;
	}

	if ((group.getMeanMagnitude() / group.getMeanDelta()) < 4.5 && group.getMeanDelta() > minDelta && group.getMeanMagnitude() > minMagnitude)
	{
		if (group.getMeanDelta() > _leadRatio * maxDelta)
			_leadRatio = group.getMeanDelta() / maxDelta;
		if (_leadRatio > 1.0)
			_leadRatio = 1.0;
	}
}

void Adapter::modifyReactionDiffusion()
{
	static float initialKillRate = _RDSimulator.getParameterValue(3)[0];
	static float initialDiffusionRateB = 0;//_RDSimulator.getParameterValue(1)[0];

	//_RDSimulator.setParameterValue(1, std::vector<float>(1, initalDiffusionRateB + (0.85 * _snareRatio)));
	//_RDSimulator.setParameterValue(3, std::vector<float>(1, initialKillRate - (0.02 * _bassRatio)));
	float tmp = initialKillRate - (0.04 * _leadRatio);
	if (tmp < 0) tmp = 0.0;
	_RDSimulator.setParameterValue(3, std::vector<float>(1, tmp));
}
