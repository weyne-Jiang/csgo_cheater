#pragma once

#include "head.hpp"
#include "ProcessController.hpp"
#include "Base.hpp"

typedef struct player
{
	int _curAddr; // ��ǰ��ҵ�ַ
	bool _isValid; // ��ǰ�ṹ���ַ�Ƿ���Ч
	bool _isSelf; // ��ǰ�ṹ���ַ�Ƿ�������
	short _camp; // ��Ӫ
	float _location[3]; // λ������
	string _name; // ����
	size_t _blood; // Ѫ��
	size_t _armor; // ����

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
	// ��ȡ��ҿ��ƶ���ָ��
	static PlayerController* getInstence();

	// ��ʼ����ַ��Ϣ 
	bool initAddr();

	// ������һ�����Ϣ
	errorType updatePlayer();

	// ������Ļ����
	errorType updateAngle();

	// ������������
	errorType updateMatrix();

	// д����Ļ����
	errorType writeAngle(float * tarAngle);

	// д��λ������
	errorType writeLocation(string name, float* tarLocation);

	// д��Ѫ��
	errorType writeBlood(string name, size_t tarBlood);

	// ��ȡ�����Ϣ
	auto getPlayer(string name);

	// ��ȡ��Ļ����
	float* getAimAngle();

	// ��ȡ��������
	float** getSelfMatrix();

	// ��ӡ����
	void showMatrix();

	PlayerController(const PlayerController& cls) = delete;
	PlayerController(PlayerController&& cls) = delete;

private:
	PlayerController();
	~PlayerController();
	//�����Զ����� RAII
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
	static PlayerController* _pInstance; // ��ҿ��ƶ���ָ��
	static AutoRelease _autoRelease;
	ProcessController* _pProcess; // ���̿��ƶ���ָ��

	LPVOID _matrixAddr; //�����ַ
	LPVOID _angleAddr; //��Ļ�ǶȻ�ַ
	LPVOID _playerAddr; //��һ�ַ

	unordered_map<string ,shared_ptr<player>> _playerMap; // ��Ҽ�ֵ��
	float _aimAngle[2]; // ��׼�Ƕȣ���Ļ���꣩
	float _selfMatrix[4][4]; // ������׼�����������꣩
	size_t _roomCount; // ��������

};
