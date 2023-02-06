#pragma once

#include "head.hpp"
#include "ProcessController.hpp"
#include "Base.hpp"

typedef struct player
{
	int _curAddr; // 当前玩家地址
	bool _isValid; // 当前结构体地址是否有效
	bool _isSelf; // 当前结构体地址是否是自身
	short _camp; // 阵营
	float _location[3]; // 位置坐标
	string _name; // 名字
	size_t _blood; // 血量
	size_t _armor; // 护甲

	player() : _isValid(false), _isSelf(false), _camp(-1), _name("unknown"), _blood(0), _armor(0),_curAddr(-1)
	{
		for (size_t i = 0; i < sizeof(_location)/sizeof(_location[0]); i++)
		{
			_location[i] = 0;
		}
	}
}* pPlayer;

class PlayerController
{
public:
	// 获取玩家控制对象指针
	static PlayerController* getInstence();

	// 初始化基址信息 
	bool initAddr();

	// 更新玩家基本信息
	errorType updatePlayer();

	// 更新屏幕坐标
	errorType updateAngle();

	// 更新世界坐标
	errorType updateMatrix();

	// 写入屏幕坐标
	errorType writeAngle(float * tarAngle);

	// 写入位置坐标
	errorType writeLocation(string name, float* tarLocation);

	// 写入血量
	errorType writeBlood(string name, size_t tarBlood);

	// 获取玩家信息
	auto getPlayer(string name);

	// 获取屏幕坐标
	float* getAimAngle();

	// 获取世界坐标
	float** getSelfMatrix();

	// 打印矩阵
	void showMatrix();

	PlayerController(const PlayerController& cls) = delete;
	PlayerController(PlayerController&& cls) = delete;

private:
	PlayerController();
	~PlayerController();
	//用于自动销毁 RAII
	class AutoRelease
	{
	public:
		AutoRelease() { };
		~AutoRelease()
		{
			if (nullptr != _pInstance)
				delete _pInstance;
		}
	};

private:
	static PlayerController* _pInstance; // 玩家控制对象指针
	static AutoRelease _autoRelease;
	ProcessController* _pProcess; // 进程控制对象指针

	LPVOID _matrixAddr; //矩阵基址
	LPVOID _angleAddr; //屏幕角度基址
	LPVOID _playerAddr; //玩家基址

	unordered_map<string ,shared_ptr<player>> _playerMap; // 玩家键值表
	float _aimAngle[2]; // 瞄准角度（屏幕坐标）
	float _selfMatrix[4][4]; // 自身瞄准矩阵（世界坐标）
	size_t _roomCount; // 房间人数

};
