#pragma once
#include "ProcessHelper.hpp"
#include "Base.hpp"

using namespace BaseData;
using namespace BaseFunc;

typedef struct player
{
	int curAddr; // ��ǰ��ҵ�ַ
	bool isValid; // ��ǰ�ṹ���ַ�Ƿ���Ч
	bool isSelf; // ��ǰ�ṹ���ַ�Ƿ�������
	uint8_t camp; // ��Ӫ
	Point3D location; // λ������
	//float location[3]; // λ������
	string name; // ����
	uint32_t blood; // Ѫ��
	uint32_t armor; // ����

	player() : isValid(false), isSelf(false), camp(-1), name("Unknown"), blood(0), armor(0), curAddr(-1) {}
	~player() = default;
}* pPlayer;

class PlayerHelper : public Single<PlayerHelper>
{
	friend class Single<PlayerHelper>;
public:

    /*!
     * @brief ��ʼ����ַ��Ϣ
     * @return �Ƿ�ɹ�
     */
	bool initAddr();

    /*!
     * @brief ������һ�����Ϣ
     * @return �Ƿ�ɹ�
     */
	bool updatePlayers();

    /*!
     * @brief ������Ļ����
     * @return �Ƿ�ɹ�
     */
    bool updateAngle();

    /*!
     * @brief ������Ļ����
     * @return �Ƿ�ɹ�
     */
    bool updateMatrix();

    /*!
     * @brief д����Ļ����
     * @param tarAngle ��ά����
     * @return �Ƿ�ɹ�
     */
    bool writeAngle(const Point2D& tarAngle);

    /*!
     * @brief д�����λ������
     * @param name �����
     * @param tarLocation ��ά����
     * @return �Ƿ�ɹ�
     */
    bool writeLocation(const string& name, const Point3D& tarLocation);

    /*!
     * @brief д�����Ѫ��
     * @param name �����
     * @param tarBlood Ѫ��
     * @return �Ƿ�ɹ�
     */
    bool writeBlood(const string& name, size_t& tarBlood);

	// ��ȡ���������Ϣ
	unordered_map<string, shared_ptr<player>> getPlayers();

    /*!
     * @brief ��ȡ�����Ϣ
     * @param name �����
     * @return ��Ҷ���ָ��
     */
	optional<shared_ptr<player>> getPlayer(const string& name) noexcept;

	// ��ȡ��Ļ����
	Point2D getAimAngle();

	// ��ȡ��������
	float** getSelfMatrix();

	// ��ӡ����
	void printMatrix();

	// ��ӡ��ҿ�����Ϣ
	void printPlayerCon();

	// ��ӡ��Ҿ�����ϢshowPlayer
	void printPlayer(string playName);

	// ��ӡ������Ҿ�����Ϣ
	void printPlayer();

	// ��ӡ��Ļ����
	void printAngle();

private:
    /*!
     * @brief ����
     */
	PlayerHelper();

    /*!
     * @brief ����
     */
	~PlayerHelper() override = default ;

private:
    shared_ptr<Process> _pProcess; // ��������
    unordered_map<string, shared_ptr<Module>> _pModule; // ����ģ��

	LPVOID _matrixAddr; // �����ַ
	LPVOID _angleAddr; // ��Ļ�ǶȻ�ַ
	LPVOID _playerAddr; // ��һ�ַ

	unordered_map<string ,shared_ptr<player>> _playerMap; // ��Ҽ�ֵ��
	Point2D _aimAngle; // ��׼�Ƕȣ���Ļ���꣩
	float _selfMatrix[4][4]; // ������׼�����������꣩
	uint8_t _roomCount; // ��������

};
