#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

constexpr auto plugin_version = "1.0";

class SpeedBoost: public BakkesMod::Plugin::BakkesModPlugin
{

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	void updateEnabled();
	void OnBoostPickUp(ActorWrapper caller, void* params, std::string funcName);
	void onTick();
};

