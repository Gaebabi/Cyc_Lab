#include "Gui.h"

void Gui::Test()
{
	static bool checkBox = false;
	static int sliderInt = 10000;
	ImGui::Begin("Window");
	ImGui::Checkbox("Toggle", &checkBox);
	//ImGui::SliderInt("Slider###SliderInt", &sliderInt, 0, 50000);
	ImGui::End();
}
