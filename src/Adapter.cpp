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

	const float minBassMagnitude = 50;
	const float maxBassMagnitude = 150;
	float bassMagnitude = 0;
	float bassDelta = 0;

	const float maxSnareMagnitude = 10;
	const float minSnareMagnitude = 2;
	float snareMagnitude = 0;

	for (int i = 0; i < (int)groups.size(); i++)
	{
		SoundGroup& group = groups[i];

		// Bass
		if (group.getMeanIndex() < 2 && group.getMeanMagnitude() > minBassMagnitude)
		{
			if (group.getMeanDelta() > bassDelta)
			{
				bassDelta = group.getMeanDelta();
				bassMagnitude = group.getMeanMagnitude();
			}
		}

		// Snare
		if (group.getBandNumber() > 6 && group.getMeanIndex() > 20 && group.getMeanMagnitude() > minSnareMagnitude)
		{
			if (group.getMeanMagnitude() > snareMagnitude)
				snareMagnitude = group.getMeanMagnitude();
		}
	}

	_bassRatio = bassMagnitude / (maxBassMagnitude - minBassMagnitude);
	if (_bassRatio > 1.0) _bassRatio = 1.0;

	_snareRatio = snareMagnitude / (maxSnareMagnitude - minSnareMagnitude);
	if (_snareRatio > 1.0) _snareRatio = 1.0;
}

void Adapter::modifyReactionDiffusion()
{
	std::vector<SoundGroup>& groups = _audioAnalyzer.getGroups();

	// magnitude * delta * cst
	// bandNb & meanIndex -> select rds propertie

	for (int i = 0; i < (int)groups.size(); i++)
	{
		//// to name accordingly ...
		//else if (group.getBandNumber() <= 3 && group.getMeanDelta() > 10)
		//{
		//	//_RDSimulator.setParameterValue(1, std::vector<float>(1, 0.1 + (0.025 * group.getMeanMagnitude())));
		//}
	}

	static float initialKillRate = _RDSimulator.getParameterValue(3)[0];
	static float initalDiffusionRateB = _RDSimulator.getParameterValue(1)[0];

	_RDSimulator.setParameterValue(1, std::vector<float>(1, initalDiffusionRateB + (0.45 * _snareRatio)));
	_RDSimulator.setParameterValue(3, std::vector<float>(1, initialKillRate - (0.03 * _bassRatio)));
}
