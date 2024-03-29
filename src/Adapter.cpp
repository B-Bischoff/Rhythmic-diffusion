#include "Adapter.hpp"
#include "vendor/imgui/imgui.h"

Adapter::Adapter(ReactionDiffusionSimulator& RDSimulator, AudioAnalyzer& audioAnalyzer)
	: _RDSimulator(RDSimulator), _audioAnalyzer(audioAnalyzer)
{ }

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

void Adapter::createHook(const AudioTrigger audioTrigger, const int reactionPropertie, const int propertieIndex, const ActionMode actionMode, const double simulationInitialValue, const double value)
{
	if (reactionPropertie < 4)
	{
		try {
			const std::vector<float>& vector = _RDSimulator.getParameterValue(reactionPropertie);
			(void)vector.at(propertieIndex);
		} catch (std::invalid_argument& e) {
			std::cerr << "[Adapter]: could not create hook, invalid reaction propertie" << std::endl;
			return;
		} catch (std::out_of_range& e) {
			std::cerr << "[Adapter]: could not create hook, invalid reaction propertie index" << std::endl;
			return;
		}
	}

	_hooks.push_back(AdapterHook(audioTrigger, reactionPropertie, propertieIndex, actionMode, simulationInitialValue, value));
}

void Adapter::modifyReactionDiffusion()
{
	for (int i = 0; i < (int)_hooks.size(); i++)
	{
		try {
			applyHook(_hooks[i]);
		}
		catch (...)
		{
			std::cerr << "[AdapterHook] invalid hook, removing it" << std::endl;
			removeHook(i);
		}
	}
}

void Adapter::applyHook(const AdapterHook& hook)
{
	const float& ratio = getRatioReferenceFromAudioTrigger(hook.audioTrigger);

	float newValue;
	// Use fuction pointer
	if (hook.actionMode == add)
		newValue = hook.simulationInitialValue + (ratio * hook.value);
	else if (hook.actionMode == subtract)
		newValue = hook.simulationInitialValue - (ratio * hook.value);
	else
		newValue = ratio >= 1.0 ? hook.value : hook.simulationInitialValue;

	// Reaction diffusion parameter ( rdA, rdB, feedRate or killRate )
	const int REACTION_DIFFUSION_PARAMETER_NB = 4;
	if (hook.reactionPropertie >= 0 && hook.reactionPropertie < REACTION_DIFFUSION_PARAMETER_NB)
	{
		std::vector<float> newVector = _RDSimulator.getParameterValue(hook.reactionPropertie);
		if (hook.propertieIndex >= (int)newVector.size())
			return;
		newVector[hook.propertieIndex] = newValue;
		_RDSimulator.setParameterValue(hook.reactionPropertie, newVector);
	}
	else // Reaction diffusion intial conditions shape
	{
		InitialConditionsShape& shape = _RDSimulator.getInitialConditionsShapes().at(hook.reactionPropertie - REACTION_DIFFUSION_PARAMETER_NB) ;
		switch (hook.propertieIndex)
		{
			case 0: shape.radius = newValue; break;
			case 1: shape.borderSize = newValue; break;
			case 2: shape.rotationAngle = newValue; break;
			case 3: shape.offset.x = newValue; break;
			case 4: shape.offset.y = newValue; break;
		}
	}
}

const float& Adapter::getRatioReferenceFromAudioTrigger(const AudioTrigger& audioTrigger) const
{
	if (audioTrigger == AudioTrigger::bass)
		return _bassRatio;
	else if (audioTrigger == AudioTrigger::snare)
		return _snareRatio;
	else
		return _leadRatio;
}

void Adapter::removeHook(const int& index)
{
	int i = 0;
	for (std::vector<AdapterHook>::iterator it = _hooks.begin(); it != _hooks.end(); it++)
	{
		if (i == index)
		{
			_hooks.erase(it);
			return;
		}
		i++;
	}
}

void Adapter::clearHooks()
{
	_hooks.clear();
}

std::vector<AdapterHook>& Adapter::getHooks()
{
	return _hooks;
}

glm::vec3 Adapter::getRatios() const
{
	return glm::vec3(_bassRatio, _snareRatio, _leadRatio);
}
