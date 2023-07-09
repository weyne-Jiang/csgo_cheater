#pragma once
#include "head.hpp"
#include "Single.hpp"

#ifdef _DEBUG
/*!
 * @brief ����׷��
 */
#define MACRO_TRACE(format, ...) \
	printf("File: \"%s\", Function: %s, Line: %d -> ", __FILE__, __FUNCTION__, __LINE__);\
	printf(format, ##__VA_ARGS__)
#else
#define MACRO_TRACE(format, ...)
#endif // _DEBUG

namespace BaseFunc
{
    /*!
     * @brief �����ж�
     * @param content ��������
     */
	void errorInfo(const std::string& content);

    /*!
     * @brief �����ж�
     * @param content ��������
     */
	void warningInfo(const std::string& content);

    /*!
     * @brief ��Զ�̽����ڴ�
     * @param handle ���̾��
     * @param addr ��������ʼ��ַ
     * @param pBuff ��ȡ�ڴ滺��
     * @param size Ԥ����ȡ�ߴ�
     * @return ʵ�ʶ�ȡ�ߴ�
     */
	uint64_t readMemory(HANDLE handle, LPCVOID addr, LPVOID pBuff, SIZE_T size);

    /*!
     * @brief дԶ�̽����ڴ�
     * @param handle ���̾��
     * @param addr ��������ʼ��ַ
     * @param pBuff д���ڴ滺��
     * @param size Ԥ��д��ߴ�
     * @return ʵ��д��ߴ�
     */
	uint64_t writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T size);

	/*!
	 * @brief ��ȡ�����ڴ�
	 * @tparam T ��λ����-����
	 * @param handle ���̾��
	 * @param addr �����ڴ���ʼ��ַ
	 * @param count ��ȡ��λ����
	 * @return ����ָ��
	 */
	template<typename T>
	[[nodiscard]]
	shared_ptr<T> readMemory(HANDLE handle, LPCVOID addr, SIZE_T count)
	{
        auto pRes = new T[count]();
//		shared_ptr<T> pRes(new T[count]());
		readMemory(handle, addr, (LPVOID)pRes, count * sizeof(T));
		return std::make_shared<T>(pRes);
	}

	/*!
	 * @brief д������ڴ�
	 * @tparam T ��λ����-����
	 * @param handle ���̾��
	 * @param addr �����ڴ���ʼ��ַ
	 * @param pBuff ���ݻ���
	 * @param count ��ȡ��λ����
	 * @return ʵ�ʶ�ȡ�ֽ���
	 */
	template<typename T>
	uint64_t writeMemory(HANDLE handle, LPVOID addr, LPVOID pBuff, SIZE_T count)
	{
		return writeMemory(handle, addr, pBuff, count * sizeof(T));
	}

    /*!
 * @brief ��Զ�̽����ڴ�
 * @param handle ���̾��
 * @param addr ��������ʼ��ַ
 * @param pBuff ��ȡ�ڴ滺��
 * @param size Ԥ����ȡ�ߴ�
 * @return ʵ�ʶ�ȡ�ߴ�
 */
    uint64_t readMemory(HANDLE handle, long addr, LPVOID pBuff, SIZE_T size);

    /*!
     * @brief дԶ�̽����ڴ�
     * @param handle ���̾��
     * @param addr ��������ʼ��ַ
     * @param pBuff д���ڴ滺��
     * @param size Ԥ��д��ߴ�
     * @return ʵ��д��ߴ�
     */
    uint64_t writeMemory(HANDLE handle, long addr, LPVOID pBuff, SIZE_T size);

    /*!
     * @brief ��ȡ�����ڴ�
     * @tparam T ��λ����-����
     * @param handle ���̾��
     * @param addr �����ڴ���ʼ��ַ
     * @param count ��ȡ��λ����
     * @return ����ָ��
     */
    template<typename T>
    [[nodiscard]]
    shared_ptr<T> readMemory(HANDLE handle, long addr, SIZE_T count)
    {
        auto pRes = new T[count]();
//		shared_ptr<T> pRes(new T[count]());
        readMemory(handle, addr, (LPVOID)pRes, count * sizeof(T));
        return std::make_shared<T>(pRes);
    }

    /*!
     * @brief д������ڴ�
     * @tparam T ��λ����-����
     * @param handle ���̾��
     * @param addr �����ڴ���ʼ��ַ
     * @param pBuff ���ݻ���
     * @param count ��ȡ��λ����
     * @return ʵ�ʶ�ȡ�ֽ���
     */
    template<typename T>
    uint64_t writeMemory(HANDLE handle, long addr, LPVOID pBuff, SIZE_T count)
    {
        return writeMemory(handle, addr, pBuff, count * sizeof(T));
    }


    /*!
     * @brief ���뱾�����ڴ�
     * @param size �����ֽ���
     * @param lpAddress �����������ڴ��ַ
     * @return ҳ���������Ļ�ַ
     */
    [[nodiscard]]
	LPVOID allocMemory(SIZE_T size, LPVOID lpAddress = nullptr);

    /*!
     * @brief ����Զ�̽����ڴ�
     * @param hProcess ���̾��
     * @param size �����ֽ���
     * @param lpAddress Զ�̽��������ڴ��ַ
     * @return ҳ���������Ļ�ַ
     */
    [[nodiscard]]
    LPVOID allocMemoryEx(HANDLE hProcess, SIZE_T size, LPVOID lpAddress = nullptr);

    /*!
     * @brief �ͷű������ڴ�
     * @param pAddr ָ��Ҫ�ͷŵ�ҳ������Ļ�ַ��ָ��
     */
    void freeMemory(LPVOID pAddr);

    /*!
     * @brief �ͷ�Զ�̽����ڴ�
     * @param hProcess ���̾��
     * @param pAddr ָ��Ҫ�ͷŵ�ҳ������Ļ�ַ��ָ��
     */
    void freeMemory(HANDLE hProcess, LPVOID pAddr);

    /*!
     * @brief ��ӡ����
     * @tparam Types ���ù�
     * @param args ���ù�
     */
    template <typename ... Types>
    void print (const Types&... args)
    {
#ifdef _DEBUG
        std::initializer_list <int> { ([&args] {std::cout << args;}(), 0)...};
        std::cout << std::endl;
#endif
    }
}

namespace BaseData
{
    class Point3D {
    public:
        Point3D()
        {
            _pPoint = new float [3]();
            _pPoint[0] = 0;
            _pPoint[1] = 0;
            _pPoint[2] = 0;
        }
    
        explicit Point3D(const float p[3])
        {
            _pPoint = new float [3]();
            _pPoint[0] = p[0];
            _pPoint[1] = p[1];
            _pPoint[2] = p[2];
        }

        Point3D(const float& x, const float& y, const float& z)
        {
            _pPoint = new float [3]();
            _pPoint[0] = x;
            _pPoint[1] = y;
            _pPoint[2] = z;
        }

        ~Point3D()
        {
            delete [] _pPoint;
        }

        [[nodiscard]]
        float& x() const
        {
            return _pPoint[0];
        }

        [[nodiscard]]
        float& y() const
        {
            return _pPoint[1];
        }

        [[nodiscard]]
        float& z() const
        {
            return _pPoint[2];
        }

        [[nodiscard]]
        float * floatPoint() const
        {
            return _pPoint;
        }

        //������ĳһ��ľ���
        float distance(Point3D& point) const
        {
            return sqrtf((point.x() - this->x()) * (point.x() - this->x()) + (point.y() - this->y()) * (point.y() - this->y()) + (point.z() - this->z()) * (point.z() - this->z()));
        }

        // �����������
        static float distance(Point3D& p1, Point3D& p2) {
            return sqrtf((p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()) + (p1.z()-p2.z())*(p1.z()-p2.z()));
        }

        // ���ؼӷ� 
        Point3D operator + (const Point3D& cls) const
        {
            return {this->x() + cls.x(), this->y() + cls.y(), this->z() + cls.z()};
        }

        // ���ؼ���
        Point3D operator - (const Point3D& cls) const
        {
            return {this->x() - cls.x(), this->y() - cls.y(), this->z() - cls.z()};
        }

        // �����±�
        float & operator [] (const int& index) {
            if (index < 0 || index > 2)
                BaseFunc::errorInfo("�±����Խ��");

            switch (index)
            {
            case 0:
                return _pPoint[0];
            case 1:
                return _pPoint[1];
            case 2:
                return _pPoint[2];
            default:
                break;
            }
        }

        //������������
        friend std::ostream& operator << (std::ostream& os, const Point3D& p) {
            os << '(' << p.x() << ',' << p.y() << ',' << p.z() << ')';
            return os;
        }

    private:
        float *_pPoint;
    };

    class Point2D
    {
    public:
        Point2D()
        {
            _pPoint = new float [2]();
            _pPoint[0] = 0;
            _pPoint[1] = 0;
        }

        explicit Point2D(const float p[2])
        {
            _pPoint = new float [2]();
            _pPoint[0] = p[0];
            _pPoint[1] = p[1];
        }

        Point2D(const float& x, const float& y)
        {
            _pPoint = new float [2]();
            _pPoint[0] = x;
            _pPoint[1] = y;
        }

        ~Point2D()
        {
            delete [] _pPoint;
        }

        [[nodiscard]]
        float * floatPoint() const
        {
            return _pPoint;
        }

        [[nodiscard]]
        float& x() const
        {
            return _pPoint[0];
        }

        [[nodiscard]]
        float& y() const
        {
            return _pPoint[1];
        }

        //������ĳһ��ľ���
        float distance(Point2D& point) const
        {
            return sqrtf((point.x() - this->x()) * (point.x() - this->x()) + (point.y() - this->y()) * (point.y() - this->y()));
        }

        // �����������
        static float distance(Point2D& p1, Point2D& p2)
        {
            return sqrtf((p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()));
        }

        // ���ؼӷ�
        Point2D operator + (const Point2D& cls) const
        {
            return {this->x() + cls.x(), this->y() + cls.y()};
        }

        // ���ؼ���
        Point2D operator - (const Point2D& cls) const
        {
            return {this->x() - cls.x(), this->y() - cls.y()};
        }

        // �����±�
        float & operator [] (const int& index) {
            if (index < 0 || index > 1)
                BaseFunc::errorInfo("�±����Խ��");

            switch (index)
            {
                case 0:
                    return _pPoint[0];
                case 1:
                    return _pPoint[1];
                default:
                    break;
            }
        }

        //������������
        friend std::ostream& operator << (std::ostream& os, const Point2D& p) {
            os << '(' << p.x() << ',' << p.y() << ')';
            return os;
        }

    private:
        float *_pPoint;
    };
}