#include "./AudioPlayerUI.hpp"

AudioPlayerUI::AudioPlayerUI(ImGui::FileBrowser& fileBrowser, AudioPlayer& audioPlayer, AudioAnalyzer& audioAnalyzer)
	: _fileBrowser(fileBrowser), _audioPlayer(audioPlayer), _audioAnalyzer(audioAnalyzer)
{
}

void AudioPlayerUI::print()
{
	// File explorer
	if (ImGui::Button("open file explorer"))
		_fileBrowser.Open();

	// PAUSE / RESUME
	ImGui::SameLine();
	ImGui::Text("audio file: %s", _audioPlayer.getFileNameWithoutPath().c_str());
	const std::string pauseResumeText = _audioPlayer.isPlaying() ? "Pause" : "Resume";
	if (ImGui::Button(pauseResumeText.c_str()))
	{
		if (_audioPlayer.getStreamOpened())
			_audioPlayer.togglePause();
		else if (!_audioPlayer.getFileName().empty())
			_audioPlayer.playWavFile(_audioPlayer.getFileName());
	}

	// VOLUME
	float volume = _audioPlayer.getVolume();
	ImGui::SliderFloat("volume", &volume, 0, 100);
	if (volume != _audioPlayer.getVolume())
		_audioPlayer.setVolume(volume);

	// TIMESTAMP
	float audioDuration = _audioPlayer.getWavFileDuration() / 44100.0; // In seconds
	float currentDuration = _audioPlayer.getCurrentTimestamp() / 44100.0; // In seconds
	float modifiedDuration = currentDuration;

	ImGui::Text("%s", convertSecondsToHoursMinutesSeconds(currentDuration).c_str());
	ImGui::SameLine(70);
	if (ImGui::SliderFloat(convertSecondsToHoursMinutesSeconds(audioDuration).c_str(), &modifiedDuration, 0, audioDuration, ""))
	{
		if (_audioPlayer.getStreamOpened())
			_audioPlayer.setTimestamp((modifiedDuration - currentDuration) * 16384);
		else if (!_audioPlayer.getFileName().empty())
			_audioPlayer.playWavFile(_audioPlayer.getFileName());
	}

	// GRAPH
	const int ARRAY_SIZE = _audioAnalyzer.getOutputArraySize();
	float ARRAY_2[ARRAY_SIZE];
	const std::vector<float>& outputFreq = _audioAnalyzer.getFrequencies();
	if ((int)outputFreq.size() >= ARRAY_SIZE)
	{
		for (int i = 0; i < ARRAY_SIZE; i++)
			ARRAY_2[i] = outputFreq[i];
		ImGui::PlotHistogram("Histogram", ARRAY_2, ARRAY_SIZE, 0, NULL, 0.0f, 50.0f, ImVec2(500, 80.0f));
	}

	ImGui::Text("\n\n");
}

std::string AudioPlayerUI::convertSecondsToHoursMinutesSeconds(const int& seconds) const
{
	int n = seconds;
	n = n % (24 * 3600);
	int hour = n / 3600;

	n %= 3600;
	int minutes = n / 60 ;

	n %= 60;
	int secondsRemaining = n;

	std::string result;
	if (seconds >= 3600)
		result += std::to_string(hour) + ":"; // hours
	if (minutes < 10)
		result += "0";
	result +=  std::to_string(minutes) + ":"; // minutes

	if (secondsRemaining < 10)
		result += "0";
	result += std::to_string(secondsRemaining); // seconds
	return result;
}
