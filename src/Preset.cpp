#include "Preset.hpp"

Preset::Preset(ReactionDiffusionSimulator& RDSsimulator, Adapter& adapter)
	: _RDSsimulator(RDSsimulator), _adapter(adapter)
{
}

void Preset::addPreset(std::string presetName)
{
	if (presetExists(presetName))
	{
		std::cerr << "[Preset] a preset named \"" << presetName << "\" already exists. Canceling add preset method." << std::endl;
		return;
	}
	std::cout << _adapter.getHooks().size() << std::endl;

	if (presetName.empty() || presetName.find_first_not_of(' ') == std::string::npos)
		presetName = createGenericName();

	_presets[presetName] = PresetSettings();

	PresetSettings& presetSettings = _presets[presetName];
	presetSettings.hooks = _adapter.getHooks();
	presetSettings.parameters = _RDSsimulator.getParametersValue();
	presetSettings.shapes = _RDSsimulator.getInitialConditionsShapes();
}

void Preset::removePreset(const std::string& presetName)
{
	if (!presetExists(presetName))
	{
		std::cerr << "[Preset] preset named \"" << presetName << "\" does not exists. Canceling remove preset method." << std::endl;
		return;
	}
	_presets.erase(presetName);
}

bool Preset::presetExists(const std::string& presetName) const
{
	return _presets.find(presetName) != _presets.end();
}

void Preset::applyPreset(const std::string& presetName)
{
	if (!presetExists(presetName))
	{
		std::cerr << "[Preset] preset named \"" << presetName << "\" does not exists. Canceling apply preset method." << std::endl;
		return;
	}

	// Clear simulation parameters
	_adapter.clearHooks();
	_RDSsimulator.clearInitialConditionsShapes();

	PresetSettings& presetSettings = _presets[presetName];

	// Reaction diffusion parameters
	for (int i = 0; i < (int)presetSettings.parameters.size(); i++)
	{
		Parameter& parameter = presetSettings.parameters[i];
		_RDSsimulator.setParameterType(i, parameter.type);
		_RDSsimulator.setParameterValue(i, parameter.parameters);
	}

	// Hooks
	for (int i = 0; i < (int)presetSettings.hooks.size(); i++)
	{
		AdapterHook& hook = presetSettings.hooks[i];
		_adapter.createHook(hook.audioTrigger,
			hook.reactionPropertie,
			hook.propertieIndex,
			hook.actionMode,
			hook.simulationInitialValue,
			hook.value
		);
	}

	// Initial conditions shapes
	for (int i = 0; i < (int)presetSettings.shapes.size(); i++)
	{
		InitialConditionsShape& shape = presetSettings.shapes[i];
		_RDSsimulator.addInitialConditionsShape(shape);
	}
}

std::vector<std::string> Preset::getPresetNames() const
{
	std::vector<std::string> presetNames;

	std::map<std::string, PresetSettings>::const_iterator it = _presets.begin();
	for (; it != _presets.end(); it++)
		presetNames.push_back(it->first);

	return presetNames;
}

std::string Preset::createGenericName() const
{
	int i = 0;
	std::string presetName = "preset " + std::to_string(i);

	while (presetExists(presetName))
	{
		i++;
		presetName = "preset " + std::to_string(i);
	}
	return presetName;
}
