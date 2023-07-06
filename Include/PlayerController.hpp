#pragma once

#include "ProcessHelper.hpp"
#include "Base.hpp"

using Point2D = BaseData::Point2D;
using Point3D = BaseData::Point3D;

typedef struct player
{
	int _curAddr; // 当前玩家地址
	bool _isValid; // 当前结构体地址是否有效
	bool _isSelf; // 当前结构体地址是否是自身
	short _camp; // 阵营
	Point3D _location; // 位置坐标
	//float _location[3]; // 位置坐标
	string _name; // 名字
	size_t _blood; // 血量
	size_t _armor; // 护甲

	player() : _isValid(false), _isSelf(false), _camp(-1), _name("unknown"), _blood(0), _armor(0),_curAddr(-1)
	{
	}
	~player()
	{
	}
}* pPlayer;

class PlayerController : public Single<PlayerController>
{
	friend class Single<PlayerController>;
public:

	// 初始化基址信息 
	bool initAddr();

	// 更新玩家基本信息
	errorType readPlayer();

	// 更新屏幕坐标
	errorType readAngle();

	// 更新世界坐标
	errorType readMatrix();

	// 写入屏幕坐标
	errorType writeAngle(Point2D& tarAngle);

	// 写入位置坐标
	errorType writeLocation(const string& name, Point3D& tarLocation);

	// 写入血量
	errorType writeBlood(const string& name, size_t& tarBlood);

	// 获取所有玩家信息
	unordered_map<string, shared_ptr<player>> getPlayer();

	// 获取玩家信息
	shared_ptr<player> getPlayer(string name);

	// 获取屏幕坐标
	Point2D getAimAngle();

	// 获取世界坐标
	float** getSelfMatrix();

	// 打印矩阵
	void printMatrix();

	// 打印玩家控制信息
	void printPlayerCon();

	// 打印玩家具体信息showPlayer
	void printPlayer(string playName);

	// 打印所有玩家具体信息
	void printPlayer();

	// 打印屏幕坐标
	void printAngle();

private:
	PlayerController();
	~PlayerController();

private:
	Process* _pProcess; // 进程控制对象指针

	LPVOID _matrixAddr; //矩阵基址
	LPVOID _angleAddr; //屏幕角度基址
	LPVOID _playerAddr; //玩家基址

	unordered_map<string ,shared_ptr<player>> _playerMap; // 玩家键值表
	Point2D _aimAngle; // 瞄准角度（屏幕坐标）
	float _selfMatrix[4][4]; // 自身瞄准矩阵（世界坐标）
	size_t _roomCount; // 房间人数

};
