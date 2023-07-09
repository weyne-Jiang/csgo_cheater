#pragma once
#include "ProcessHelper.hpp"
#include "Base.hpp"

using namespace BaseData;
using namespace BaseFunc;

typedef struct player
{
	int curAddr; // 当前玩家地址
	bool isValid; // 当前结构体地址是否有效
	bool isSelf; // 当前结构体地址是否是自身
	uint8_t camp; // 阵营
	Point3D location; // 位置坐标
	//float location[3]; // 位置坐标
	string name; // 名字
	uint32_t blood; // 血量
	uint32_t armor; // 护甲

	player() : isValid(false), isSelf(false), camp(-1), name("Unknown"), blood(0), armor(0), curAddr(-1) {}
	~player() = default;
}* pPlayer;

class PlayerHelper : public Single<PlayerHelper>
{
	friend class Single<PlayerHelper>;
public:

    /*!
     * @brief 初始化基址信息
     * @return 是否成功
     */
	bool initAddr();

    /*!
     * @brief 更新玩家基本信息
     * @return 是否成功
     */
	bool updatePlayers();

    /*!
     * @brief 更新屏幕坐标
     * @return 是否成功
     */
    bool updateAngle();

    /*!
     * @brief 更新屏幕坐标
     * @return 是否成功
     */
    bool updateMatrix();

    /*!
     * @brief 写入屏幕坐标
     * @param tarAngle 二维坐标
     * @return 是否成功
     */
    bool writeAngle(const Point2D& tarAngle);

    /*!
     * @brief 写入玩家位置坐标
     * @param name 玩家名
     * @param tarLocation 三维坐标
     * @return 是否成功
     */
    bool writeLocation(const string& name, const Point3D& tarLocation);

    /*!
     * @brief 写入玩家血量
     * @param name 玩家名
     * @param tarBlood 血量
     * @return 是否成功
     */
    bool writeBlood(const string& name, size_t& tarBlood);

	// 获取所有玩家信息
	unordered_map<string, shared_ptr<player>> getPlayers();

    /*!
     * @brief 获取玩家信息
     * @param name 玩家名
     * @return 玩家对象指针
     */
	optional<shared_ptr<player>> getPlayer(const string& name) noexcept;

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
    /*!
     * @brief 构造
     */
	PlayerHelper();

    /*!
     * @brief 析构
     */
	~PlayerHelper() override = default ;

private:
    shared_ptr<Process> _pProcess; // 所处进程
    unordered_map<string, shared_ptr<Module>> _pModule; // 所含模块

	LPVOID _matrixAddr; // 矩阵基址
	LPVOID _angleAddr; // 屏幕角度基址
	LPVOID _playerAddr; // 玩家基址

	unordered_map<string ,shared_ptr<player>> _playerMap; // 玩家键值表
	Point2D _aimAngle; // 瞄准角度（屏幕坐标）
	float _selfMatrix[4][4]; // 自身瞄准矩阵（世界坐标）
	uint8_t _roomCount; // 房间人数

};
