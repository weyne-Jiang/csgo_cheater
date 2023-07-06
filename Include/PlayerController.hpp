#pragma once

#include "ProcessHelper.hpp"
#include "Base.hpp"

using Point2D = BaseData::Point2D;
using Point3D = BaseData::Point3D;

typedef struct player
{
	int _curAddr; // ��ǰ��ҵ�ַ
	bool _isValid; // ��ǰ�ṹ���ַ�Ƿ���Ч
	bool _isSelf; // ��ǰ�ṹ���ַ�Ƿ�������
	short _camp; // ��Ӫ
	Point3D _location; // λ������
	//float _location[3]; // λ������
	string _name; // ����
	size_t _blood; // Ѫ��
	size_t _armor; // ����

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

	// ��ʼ����ַ��Ϣ 
	bool initAddr();

	// ������һ�����Ϣ
	errorType readPlayer();

	// ������Ļ����
	errorType readAngle();

	// ������������
	errorType readMatrix();

	// д����Ļ����
	errorType writeAngle(Point2D& tarAngle);

	// д��λ������
	errorType writeLocation(const string& name, Point3D& tarLocation);

	// д��Ѫ��
	errorType writeBlood(const string& name, size_t& tarBlood);

	// ��ȡ���������Ϣ
	unordered_map<string, shared_ptr<player>> getPlayer();

	// ��ȡ�����Ϣ
	shared_ptr<player> getPlayer(string name);

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
	PlayerController();
	~PlayerController();

private:
	Process* _pProcess; // ���̿��ƶ���ָ��

	LPVOID _matrixAddr; //�����ַ
	LPVOID _angleAddr; //��Ļ�ǶȻ�ַ
	LPVOID _playerAddr; //��һ�ַ

	unordered_map<string ,shared_ptr<player>> _playerMap; // ��Ҽ�ֵ��
	Point2D _aimAngle; // ��׼�Ƕȣ���Ļ���꣩
	float _selfMatrix[4][4]; // ������׼�����������꣩
	size_t _roomCount; // ��������

};
