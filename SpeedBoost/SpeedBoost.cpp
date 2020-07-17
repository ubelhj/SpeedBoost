#include "pch.h"
#include "SpeedBoost.h"


BAKKESMOD_PLUGIN(SpeedBoost, "Boost your speed with boost pads", plugin_version, PLUGINTYPE_FREEPLAY)

bool enabled;
float speedBoost;
float maxBoost;

void SpeedBoost::onLoad()
{
	// cvars to allow customizability
	auto enabledCvar = cvarManager->registerCvar("speedboost_enabled", "0", "Enables or disables speed pads");
	enabled = enabledCvar.getBoolValue();
	enabledCvar.addOnValueChanged([this](std::string, CVarWrapper cvar) { enabled = cvar.getBoolValue(); updateEnabled(); });

	auto boostCvar = cvarManager->registerCvar("speedboost_power", "0.25", "How much of your speed the boost pad adds");
	speedBoost = boostCvar.getFloatValue();
	boostCvar.addOnValueChanged([this](std::string, CVarWrapper cvar) { speedBoost = cvar.getFloatValue(); });

	auto maxBoostCvar = cvarManager->registerCvar("speedboost_maxboost", "0", "Maximum boost a car can have", true, true, 0, true, 100);
	setMaxBoost(maxBoostCvar.getIntValue());
	maxBoostCvar.addOnValueChanged([this](std::string, CVarWrapper cvar) { setMaxBoost(cvar.getIntValue()); });
	
}

// structure of boost pickup event
struct TheArgStruct
{
	// car that recieved the pickup
	uintptr_t Receiver;
};

void SpeedBoost::updateEnabled() {
	if (enabled) {
		// on enable hooks game events
		// happens on boost pickup
		gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.VehiclePickup_Boost_TA.Pickup", std::bind(&SpeedBoost::OnBoostPickUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
		// happens each tick
		gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", std::bind(&SpeedBoost::onTick, this));
	}
	else {
		gameWrapper->UnhookEvent("Function TAGame.VehiclePickup_Boost_TA.Pickup");
		gameWrapper->UnhookEvent("Function TAGame.Car_TA.SetVehicleInput");
	}
}

// called on each boost pickup
void SpeedBoost::OnBoostPickUp(ActorWrapper caller, void* params, std::string funcName) {
	//cvarManager->log(funcName);

	// converts caller to a car
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

	// simply adds velocity to the car based on the cvar
	auto carVelocity = receiver.GetVelocity();
	carVelocity.operator*(speedBoost);
	receiver.AddVelocity(carVelocity);
}

// called each tick
void SpeedBoost::onTick() {
	//cvarManager->log("countdown");
	auto sw = gameWrapper->GetGameEventAsServer();
	if (sw.IsNull()) {
		//cvarManager->log("null server");
		return;
	}

	auto cars = sw.GetCars();

	// checks if each car has more than max boost, if so reduces the boost to max
	for (int i = 0; i < cars.Count(); i++) {
		auto car = cars.Get(i);
		if (car.IsNull()) {
			//cvarManager->log("null car");
			return;
		}
		auto boost = car.GetBoostComponent();
		if (!boost.IsNull()) {
			if (boost.GetCurrentBoostAmount() > maxBoost) {
				boost.SetBoostAmount(maxBoost);
			}
		}
	}
}

// used by Cvar to set the boost to a proper float (0.0-1.0) from an int (0-100)
void SpeedBoost::setMaxBoost(int newBoost) {
	maxBoost = (float)newBoost / 100.0;
}

void SpeedBoost::onUnload()
{
}