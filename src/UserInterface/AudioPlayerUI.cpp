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
	ImGui::Text("audio file: %s", _audioPlayer.getFileName().c_str());
	const std::string pauseResumeText = _audioPlayer.isPlaying() ? "Pause" : "Resume";
	if (ImGui::Button(pauseResumeText.c_str()))
		_audioPlayer.togglePause();

	// VOLUME
	float volume = _audioPlayer.getVolume();
	ImGui::SliderFloat("volume", &volume, 0, 100);
	if (volume != _audioPlayer.getVolume())
		_audioPlayer.setVolume(volume);

	// TIMESTAMP
	float audioDuration = _audioPlayer.getWavFileDuration() / 44100.0; // In seconds
	float currentDuration = _audioPlayer.getCurrentTimestamp() / 44100.0; // In seconds
	float modifiedDuration = currentDuration;
	ImGui::SliderFloat("timestamp", &modifiedDuration, 0, audioDuration);
	if (modifiedDuration != currentDuration)
		_audioPlayer.setTimestamp((modifiedDuration - currentDuration) * 16384);

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

}
