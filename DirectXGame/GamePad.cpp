#include "GamePad.h"
#include<imgui.h>
#include <limits>
#include <algorithm>
#include "MyMatrix.h"
#include "MyVector.h"

GamePad::GamePad() : preButton(0), state({0}), vibration({0}) {}

GamePad* GamePad::GetInstans() {
	static GamePad pInstans;
	return &pInstans;
}

void GamePad::Input() {
	GamePad::GetInstans()->preButton = GamePad::GetInstans()->state.Gamepad.wButtons;
	XInputGetState(0, &GamePad::GetInstans()->state);
}

bool GamePad::GetButton(Button type) {
	return (GamePad::GetInstans()->state.Gamepad.wButtons >> static_cast<short>(type)) % 2 == 1;
}

bool GamePad::GetPreButton(Button type) {
	return (GamePad::GetInstans()->preButton >> static_cast<short>(type)) % 2 == 1;
}

bool GamePad::Trigered(Button type) { return GetButton(type) && !GetPreButton(type); }

bool GamePad::Pushed(Button type) { return GetButton(type) && GetPreButton(type); }

bool GamePad::Released(Button type) { return !GetButton(type) && GetPreButton(type); }

float GamePad::GetTriger(Triger type) {
	static constexpr float kNormal = 1.0f / static_cast<float>(UCHAR_MAX);

	switch (type) {
	case GamePad::Triger::LEFT:
		return static_cast<float>(GamePad::GetInstans()->state.Gamepad.bLeftTrigger) * kNormal;
		break;

	case GamePad::Triger::RIGHT:
		return static_cast<float>(GamePad::GetInstans()->state.Gamepad.bRightTrigger) * kNormal;
		break;

	default:
		return 0.0f;
		break;
	}
}

float GamePad::GetStick(Stick type) {
	static constexpr float kNormal = 1.0f / static_cast<float>(SHRT_MAX);

	switch (type) {
	case GamePad::Stick::LEFT_X:
		return static_cast<float>(GamePad::GetInstans()->state.Gamepad.sThumbLX) * kNormal;
		break;
	case GamePad::Stick::LEFT_Y:
		return static_cast<float>(GamePad::GetInstans()->state.Gamepad.sThumbLY) * kNormal;
		break;
	case GamePad::Stick::RIGHT_X:
		return static_cast<float>(GamePad::GetInstans()->state.Gamepad.sThumbRX) * kNormal;
		break;
	case GamePad::Stick::RIGHT_Y:
		return static_cast<float>(GamePad::GetInstans()->state.Gamepad.sThumbRY) * kNormal;
		break;
	default:
		return 0.0f;
		break;
	}
}

void GamePad::Vibration(float leftVibIntensity, float rightVibIntensity) {
	leftVibIntensity = std::clamp(leftVibIntensity, 0.0f, 1.0f);
	rightVibIntensity = std::clamp(rightVibIntensity, 0.0f, 1.0f);

	GamePad::GetInstans()->vibration.wLeftMotorSpeed =
	    static_cast<WORD>(static_cast<float>(USHRT_MAX) * leftVibIntensity);
	GamePad::GetInstans()->vibration.wRightMotorSpeed =
	    static_cast<WORD>(static_cast<float>(USHRT_MAX) * rightVibIntensity);
	XInputSetState(0, &GamePad::GetInstans()->vibration);
}

