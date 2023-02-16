#include "Adapter.hpp"

Adapter::Adapter(ReactionDiffusionSimulator& RDSimulator, AudioAnalyzer& audioAnalyzer)
	: _RDSimulator(RDSimulator), _audioAnalyzer(audioAnalyzer)
{
}

void Adapter::update()
{
	std::vector<SoundGroup>& groups = _audioAnalyzer.getGroups();

	for (int i = 0; i < (int)groups.size(); i++)
	{
		SoundGroup& group = groups[i];

		if (group.getMeanDelta() * group.getBandNumber() < 6)
			continue;

		//std::cout << "group: " << i << " ";
		//std::cout << "mean delta: " << group.getMeanDelta() << " ";
		//std::cout << "mean index: " << group.getMeanIndex() << " ";
		//std::cout << "band number: " << group.getBandNumber() << std::endl;

		static float val1 = _RDSimulator.getParameterValue(3)[0];
		_RDSimulator.setParameterValue(3, std::vector<float>(1, val1));
		_RDSimulator.setParameterValue(1, std::vector<float>(1, 0.1));

		// Find bass
		if (group.getMeanIndex() < 5)
		{
			//std::cout << "bass" << std::endl;
			_RDSimulator.setParameterValue(3, std::vector<float>(1, val1 - (0.000075 * group.getMeanMagnitude())));
		}
		// Find snare
		else if (group.getMeanIndex() > 10 && group.getBandNumber() > 7)
		{
			//std::cout << "snare" << std::endl;
		}
		// to name accordingly ...
		else if (group.getBandNumber() <= 3 && group.getMeanDelta() > 10)
		{
			_RDSimulator.setParameterValue(1, std::vector<float>(1, 0.1 + (0.025 * group.getMeanMagnitude())));

		}
	}
}
