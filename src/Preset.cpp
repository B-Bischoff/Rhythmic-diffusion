#include "Preset.hpp"

Preset::Preset(ReactionDiffusionSimulator& RDSsimulator, Adapter& adapter)
	: _RDSsimulator(RDSsimulator), _adapter(adapter)
{
	_automaticPresetSwitch = false;
	loadExistingPresets();
	_currentPreset = "";
	_presetSwitchDelay = 5;
}

void Preset::addPreset(std::string presetName)
{
	if (presetExists(presetName))
	{
		std::cerr << "[Preset] a preset named \"" << presetName << "\" already exists. Canceling add preset method." << std::endl;
		return;
	}

	if (presetName.empty() || presetName.find_first_not_of(' ') == std::string::npos)
		presetName = createGenericName();

	// Create preset in map
	_presets[presetName] = PresetSettings();
	PresetSettings& presetSettings = _presets[presetName];
	presetSettings.hooks = _adapter.getHooks();
	presetSettings.parameters = _RDSsimulator.getParametersValue();
	presetSettings.shapes = _RDSsimulator.getInitialConditionsShapes();
	presetSettings.gradient = std::vector<glm::vec4>(_RDSsimulator.getPostProcessingGradient());

	// Save to a local file
	const std::string presetFile = PRESET_DIRECTORY + presetName + PRESET_EXTENSION;
	std::ofstream os(presetFile);
	if (!os.is_open())
	{
		std::cerr << "[Preset] could not save preset settings to " << presetFile << std::endl;
		return;
	}

	_currentPreset = presetName;

	cereal::XMLOutputArchive archive(os);
	archive(CEREAL_NVP(presetSettings));
}

void Preset::removePreset(const std::string& presetName)
{
	if (!presetExists(presetName))
	{
		std::cerr << "[Preset] preset named \"" << presetName << "\" does not exists. Canceling remove preset method." << std::endl;
		return;
	}
	_presets.erase(presetName);

	const std::string presetFile = PRESET_DIRECTORY + presetName + PRESET_EXTENSION;
	remove(presetFile.c_str());
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

	// Gradient
	_RDSsimulator.setPostProcessingGradient(presetSettings.gradient, 5.0);

	_currentPreset = presetName;
}

void Preset::overwritePreset(const std::string& presetName)
{
	if (!presetExists(presetName))
	{
		std::cerr << "[Preset] preset named \"" << presetName << "\" does not exists. Canceling overwrite preset method." << std::endl;
		return;
	}

	removePreset(presetName);
	addPreset(presetName);
}

std::vector<std::string> Preset::getPresetNames() const
{
	std::vector<std::string> presetNames;

	std::map<std::string, PresetSettings>::const_iterator it = _presets.begin();
	for (; it != _presets.end(); it++)
		presetNames.push_back(it->first);

	return presetNames;
}

void Preset::loadExistingPresets()
{
	_presets.clear();
	for (const auto & entry : std::filesystem::directory_iterator(PRESET_DIRECTORY))
	{
		const std::string fileName = entry.path().u8string();

		if (fileName.length() - fileName.rfind(PRESET_EXTENSION) != PRESET_EXTENSION.length()) // Check file extension
			continue;

		std::ifstream ifs(fileName);
		if (!ifs.is_open())
		{
			std::cerr << "[Preset] could not load: " << fileName << std::endl;
			continue;
		}

		std::string presetName = fileName;
		presetName.erase(presetName.end() - PRESET_EXTENSION.length(), presetName.end()); // remove file extension from prestName
		presetName.erase(presetName.begin(), presetName.begin() + PRESET_DIRECTORY.length()); // remove folder from presetName

		// Load archive data
		cereal::XMLInputArchive archive(ifs);
		PresetSettings presetSettings;
		archive(presetSettings);
		_presets[presetName] = presetSettings;
	}
}

bool Preset::presetExists(const std::string& presetName) const
{
	return _presets.find(presetName) != _presets.end();
}

std::string Preset::createGenericName() const
{
	int i = 0;
	std::string presetName = "unnamed-" + std::to_string(i);

	while (presetExists(presetName))
	{
		i++;
		presetName = "unnamed-" + std::to_string(i);
	}
	return presetName;
}

void Preset::updateAutomaticPresetSwitch()
{
	if (!_automaticPresetSwitch)
		return;
	if (_presets.size() == 0)
		return;

	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _previousTime;

	if (elapsedTime.count() < _presetSwitchDelay)
		return;

	// temporary four three manual switch
	//static int index = 0;

	//// 40 30 35 30 20 25 30 xx
	// mush le wanski
	//if (index == 0) _presetSwitchDelay = 22;
	//else if (index == 1) _presetSwitchDelay = 23;
	//else if (index == 2) _presetSwitchDelay = 23;
	//else if (index == 3) _presetSwitchDelay = 23;
	//else if (index == 4) _presetSwitchDelay = 45;
	//else if (index == 5) _presetSwitchDelay = 46;
	//else if (index == 6) _presetSwitchDelay = 45;
	//else if (index == 7) _presetSwitchDelay = 23;
	//else if (index == 8) _presetSwitchDelay = 45;
	//else if (index == 9) _presetSwitchDelay = 13;
	//else if (index == 10) _presetSwitchDelay = 32;
	//else if (index == 11) _presetSwitchDelay = 45;
	//else if (index == 12) _presetSwitchDelay = 90;

	//if (index == 0) _presetSwitchDelay = 40;
	//else if (index == 1) _presetSwitchDelay = 30;
	//else if (index == 2) _presetSwitchDelay = 35;
	//else if (index == 3) _presetSwitchDelay = 30;
	//else if (index == 4) _presetSwitchDelay = 20;
	//else if (index == 5) _presetSwitchDelay = 25;
	//else if (index == 6) _presetSwitchDelay = 27;
	//else if (index == 7) _presetSwitchDelay = 80;

	//const std::string presetName = "four-three-" + std::to_string(index);
	//index++;
	//applyPreset(presetName);

	//std::cout << "applied " << presetName << std::endl;

	std::map<std::string, PresetSettings>::iterator it;
	if (presetExists(_currentPreset))
	{
		it = _presets.find(_currentPreset);
		it++;
		if (it == _presets.end())
			it = _presets.begin();
	}
	else
		it = _presets.begin();


	applyPreset(it->first);
	_currentPreset = it->first;

	_previousTime = currentTime;
}

void Preset::setAutomaticSwitchDelay(const float& switchDelay)
{
	_presetSwitchDelay = switchDelay;
}

void Preset::startAutomaticPresetSwitch()
{
	_automaticPresetSwitch = true;
}

void Preset::stopAutomaticPresetSwitch()
{
	_automaticPresetSwitch = false;
}

bool Preset::getAutomaticPresetSwitchState() const
{
	return _automaticPresetSwitch;
}

std::string Preset::getCurrentPreset() const
{
	return _currentPreset;
}
