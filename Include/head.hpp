#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <process.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>
#include <future>
#include <random>
#include <assert.h>

using std::string;
using std::wstring;
using std::map;
using std::vector;
using std::unordered_map;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::unique_ptr;
using std::make_unique;
using std::shared_ptr;
using std::make_shared;
using std::make_pair;
using std::move;
using std::function;
using std::optional;
using std::nullopt;
using std::thread;
using std::mutex;
using namespace std::chrono;

namespace setting
{
	const string processName = "csgo.exe";
    const vector<string> moduleGroup = {"client.dll", "server.dll", "engine.dll"};
	constexpr int playerMaxCount = 32;

	//分模块定义偏移，首字母代表指针层级
	namespace offset
	{
		namespace server
		{
			constexpr ::std::ptrdiff_t a_player = 0xAAB434;
			constexpr ::std::ptrdiff_t b_blood = 0x230;
			constexpr ::std::ptrdiff_t b_posX = 0x1DC;
			constexpr ::std::ptrdiff_t b_posY = 0x1E0;
			constexpr ::std::ptrdiff_t b_posZ = 0x1E4;
			constexpr ::std::ptrdiff_t b_camp = 0x314;
			constexpr ::std::ptrdiff_t a_playcount = 0xB5C450;
		}

		namespace client
		{
			constexpr ::std::ptrdiff_t a_playerCount = 0x4DECE40;
			constexpr ::std::ptrdiff_t a_matrix = 0x4DF0D44;
			constexpr ::std::ptrdiff_t a_matrix1 = 0x4DF0E54;
		}

		namespace engine
		{
			constexpr ::std::ptrdiff_t a_aim = 0x59F194;
			constexpr ::std::ptrdiff_t b_angleY = 0x4D90;
			constexpr ::std::ptrdiff_t b_angleX = 0x4D94;
		}
	}
}

