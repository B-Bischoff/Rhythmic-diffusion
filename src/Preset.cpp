#include "Preset.hpp"

Preset::Preset(ReactionDiffusionSimulator& RDSsimulator, Adapter& adapter)
	: _RDSsimulator(RDSsimulator), _adapter(adapter)
{
	loadExistingPresets();
	_UIGradient = nullptr;
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
	_RDSsimulator.setPostProcessingGradient(presetSettings.gradient);

	// Update gradient UI
	if (_UIGradient == nullptr)
		return;

	_UIGradient->getMarks().clear();
	for (int i = 0; i < (int)presetSettings.gradient.size(); i++)
	{
		glm::vec4& gradientElem = presetSettings.gradient[i];
		ImColor color(gradientElem.x, gradientElem.y, gradientElem.z, 1.0);
		_UIGradient->addMark(gradientElem.w, color);

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

void Preset::loadExistingPresets()
{
	_presets.clear();
	for (const auto & entry : std::filesystem::directory_iterator(PRESET_DIRECTORY))
	{
		const std::string fileName = entry.path();

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

void Preset::setUIGradient(ImGradient &uiGradient)
{
	_UIGradient = &uiGradient;
}
