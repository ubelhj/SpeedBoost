#include "pch.h"
#include "SpeedBoost.h"


BAKKESMOD_PLUGIN(SpeedBoost, "Boost your speed with boost pads", plugin_version, PLUGINTYPE_FREEPLAY)

bool enabled;
float speedBoost;
int maxBoost = 0;

void SpeedBoost::onLoad()
{
	auto enabledCvar = cvarManager->registerCvar("speedboost_enabled", "0", "Enables or disables speed pads");
	enabled = enabledCvar.getBoolValue();
	enabledCvar.addOnValueChanged([this](std::string, CVarWrapper cvar) { enabled = cvar.getBoolValue(); updateEnabled(); });

	auto boostCvar = cvarManager->registerCvar("speedboost_power", "0.25", "How much of your speed the boost pad adds");
	speedBoost = boostCvar.getFloatValue();
	boostCvar.addOnValueChanged([this](std::string, CVarWrapper cvar) { speedBoost = cvar.getFloatValue(); });

	// buggy cvar
	/*
	auto maxBoostCvar = cvarManager->registerCvar("speedboost_maxboost", "0", "Maximum boost a car can have");
	maxBoost = maxBoostCvar.getIntValue();
	maxBoostCvar.addOnValueChanged([this](std::string, CVarWrapper cvar) { maxBoost = cvar.getIntValue(); });
	*/
}

struct TheArgStruct
{
	uintptr_t Receiver;
};

void SpeedBoost::updateEnabled() {
	if (enabled) {
		gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.VehiclePickup_Boost_TA.Pickup", std::bind(&SpeedBoost::OnBoostPickUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", std::bind(&SpeedBoost::onTick, this));
	}
	else {
		gameWrapper->UnhookEvent("Function TAGame.VehiclePickup_Boost_TA.Pickup");
		gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
	}
}

void SpeedBoost::OnBoostPickUp(ActorWrapper caller, void* params, std::string funcName) {
	//cvarManager->log(funcName);

	auto tArgs = (TheArgStruct*)params;

	auto receiver = CarWrapper(tArgs->Receiver);

	if (receiver.IsNull()) {
		cvarManager->log("null receiver");
		return;
	}


	if (receiver.IsNull()) {
		cvarManager->log("null car");
		return;
	}

	auto boost = receiver.GetBoostComponent();
	if (!boost.IsNull()) {
		boost.SetBoostAmount(maxBoost);
	}

	auto carVelocity = receiver.GetVelocity();
	carVelocity.operator*(speedBoost);
	receiver.AddVelocity(carVelocity);
}

void SpeedBoost::onTick() {
	//cvarManager->log("countdown");
	auto sw = gameWrapper->GetGameEventAsServer();
	if (sw.IsNull()) {
		//cvarManager->log("null server");
		return;
	}

	auto cars = sw.GetCars();

	for (int i = 0; i < cars.Count(); i++) {
		auto car = cars.Get(i);
		if (car.IsNull()) {
			//cvarManager->log("null car");
		}
		auto boost = car.GetBoostComponent();
		if (!boost.IsNull()) {
			boost.SetBoostAmount(maxBoost);
		}
	}
}

void SpeedBoost::onUnload()
{
}