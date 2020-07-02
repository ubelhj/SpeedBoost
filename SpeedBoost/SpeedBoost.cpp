#include "pch.h"
#include "SpeedBoost.h"


BAKKESMOD_PLUGIN(SpeedBoost, "Boost your speed with boost pads", plugin_version, PLUGINTYPE_FREEPLAY)


void SpeedBoost::onLoad()
{
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.VehiclePickup_Boost_TA.Pickup", std::bind(&SpeedBoost::OnBoostPickUp, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", std::bind(&SpeedBoost::onTick, this));
	//gameWrapper->HookEvent("Function TAGame.CountdownObject_TA.Countdown.EndState", std::bind(&SpeedBoost::Countdown, this));
}

struct TheArgStruct
{
	uintptr_t Receiver;
};

void SpeedBoost::OnBoostPickUp(ActorWrapper caller, void* params, std::string funcName) {
	cvarManager->log(funcName);

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
		boost.SetBoostAmount(0);
	}

	auto carVelocity = receiver.GetVelocity();
	carVelocity.operator*(0.25);
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
			break;
		}

		auto boost = car.GetBoostComponent();
		if (!boost.IsNull()) {
			//cvarManager->log("null boost");
			boost.SetBoostAmount(0);
		}
	}
}

void SpeedBoost::onUnload()
{
}