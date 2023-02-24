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

	const int minBassMagnitude = 50;
	const int maxBassMagnitude = 150;
	float bassValue = 0;
	float bassDelta = 0;

	for (int i = 0; i < (int)groups.size(); i++)
	{
		SoundGroup& group = groups[i];

		// Bass
		if (group.getMeanIndex() < 2 && group.getMeanMagnitude() > minBassMagnitude)
		{
			if (group.getMeanDelta() > bassDelta)
			{
				bassDelta = group.getMeanDelta();
				bassValue = group.getMeanMagnitude();
			}
		}
	}

	_bassRatio = bassValue / maxBassMagnitude;
	if (_bassRatio > 1.0) _bassRatio = 1.0;
}

void Adapter::modifyReactionDiffusion()
{
	std::vector<SoundGroup>& groups = _audioAnalyzer.getGroups();

	// magnitude * delta * cst
	// bandNb & meanIndex -> select rds propertie

	float bassValue = 0;
	float bassDelta = 0;
	float snareValue = 0;

	for (int i = 0; i < (int)groups.size(); i++)
	{
		SoundGroup& group = groups[i];

		// snare
		if (group.getMeanIndex() > 10 && group.getBandNumber() > 7)
		{
			float temp = group.getMeanDelta() / group.getMeanMagnitude();
			if (temp > snareValue)
				snareValue = temp;
		}

		//// Find snare
		//else if (group.getMeanIndex() > 10 && group.getBandNumber() > 7)
		//{
		//	//std::cout << "snare" << std::endl;
		//}
		//// to name accordingly ...
		//else if (group.getBandNumber() <= 3 && group.getMeanDelta() > 10)
		//{
		//	//_RDSimulator.setParameterValue(1, std::vector<float>(1, 0.1 + (0.025 * group.getMeanMagnitude())));
		//}
	}

	static float val1 = _RDSimulator.getParameterValue(3)[0];

	//std::cout << ratio << std::endl;
	_RDSimulator.setParameterValue(3, std::vector<float>(1, val1 - (0.02 * _bassRatio)));
}
