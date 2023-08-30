//
// Created by weyne-Jiang on 2023/8/5.
//
//ThreadPool.h

#pragma once
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

// Thread safe implementation of a Queue using a std::queue

template <typename T>
class SafeQueue {
private:
    std::queue<T> m_queue; //����ģ�庯���������

    std::mutex m_mutex;//���ʻ����ź���

public:
    SafeQueue() { //�չ��캯��


    }

    SafeQueue(SafeQueue& other) {//�������캯��

        //TODO:
    }

    ~SafeQueue() { //��������

    }


    bool empty() {  //�����Ƿ�Ϊ��

        std::unique_lock<std::mutex> lock(m_mutex); //�����źű�����������ֹm_queue���ı�

        return m_queue.empty();
    }

    int size() {
        std::unique_lock<std::mutex> lock(m_mutex); //�����źű�����������ֹm_queue���ı�

        return m_queue.size();
    }
//�������Ԫ��

    void enqueue(T& t) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(t);
    }
//����ȡ��Ԫ��

    bool dequeue(T& t) {
        std::unique_lock<std::mutex> lock(m_mutex); //���м���

        if (m_queue.empty()) {
            return false;
        }
        t = std::move(m_queue.front()); //ȡ������Ԫ�أ����ض���Ԫ��ֵ����������ֵ����

        m_queue.pop(); //������ӵĵ�һ��Ԫ��

        return true;
    }
};

class ThreadPool {
private:
    //�����̹߳�����
    class ThreadWorker
    {
    private:
        int m_id; //����id

        ThreadPool * m_pool;//�����̳߳�

    public:
        //���캯��

        ThreadWorker(ThreadPool * pool, const int id)
                : m_pool(pool), m_id(id) {
        }
        //����`()`����

        void operator()() {
            std::function<void()> func; //�������������func

            bool dequeued; //�Ƿ�����ȡ��������Ԫ��

            //�ж��̳߳��Ƿ�رգ�û�йرգ�ѭ����ȡ

            while (!m_pool->m_shutdown) {
                {
                    //Ϊ�̻߳����������������ʹ����̵߳����ߺͻ���

                    std::unique_lock<std::mutex> lock(m_pool->m_conditional_mutex);
                    //����������Ϊ�գ�������ǰ�߳�

                    if (m_pool->m_queue.empty()) {
                        m_pool->m_conditional_lock.wait(lock); //�ȴ���������֪ͨ�������߳�

                    }
                    //ȡ����������е�Ԫ��

                    dequeued = m_pool->m_queue.dequeue(func);
                }
                //����ɹ�ȡ����ִ�й�������

                if (dequeued) {
                    func();
                }
            }
        }
    };

    bool m_shutdown; //�̳߳��Ƿ�ر�

    SafeQueue<std::function<void()>> m_queue;//ִ�к�����ȫ���У����������

    std::vector<std::thread> m_threads; //�����̶߳���

    std::mutex m_conditional_mutex;//�߳��������������

    std::condition_variable m_conditional_lock; //�̻߳����������߳̿��Դ������߻��߻���״̬

public:
    //�̳߳ع��캯��

    ThreadPool(const int n_threads)
            : m_threads(std::vector<std::thread>(n_threads)), m_shutdown(false) {
    }

    ThreadPool(const ThreadPool &) = delete; //�������캯��������ȡ��Ĭ�ϸ��๹�캯��

    ThreadPool(ThreadPool &&) = delete; // �������캯����������ֵ����

    ThreadPool & operator=(const ThreadPool &) = delete; // ��ֵ����

    ThreadPool & operator=(ThreadPool &&) = delete; //��ֵ����

    // Inits thread pool

    void init() {
        for (int i = 0; i < m_threads.size(); ++i) {
            m_threads[i] = std::thread(ThreadWorker(this, i));//���乤���߳�
        }
    }

    // Waits until threads finish their current task and shutdowns the pool

    void shutdown() {
        m_shutdown = true;
        m_conditional_lock.notify_all(); //֪ͨ �������й����߳�

        for (int i = 0; i < m_threads.size(); ++i) {
            if(m_threads[i].joinable()) { //�ж��߳��Ƿ����ڵȴ�

                m_threads[i].join();  //���̼߳���ȴ�����

            }
        }
    }

    // Submit a function to be executed asynchronously by the pool
    //�̵߳���Ҫ����������ʹ���˺��÷������ͣ��Զ��жϺ�������ֵ

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        // Create a function with bounded parameters ready to execute
        //

        std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);//���Ӻ����Ͳ������壬���⺯������,��������ֵ����

        // Encapsulate it into a shared ptr in order to be able to copy construct // assign
        //��װ��ȡ������󣬷�������һ���̲߳鿴���

        auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

        // Wrap packaged task into void function
        //����������ʽ������һ����������

        std::function<void()> wrapper_func = [task_ptr]() {
            (*task_ptr)();
        };

        // ����ͨ�ð�ȫ�����������ѹ�밲ȫ����

        m_queue.enqueue(wrapper_func);

        // ����һ���ȴ��е��߳�

        m_conditional_lock.notify_one();

        // ������ǰע�������ָ��

        return task_ptr->get_future();
    }
};

#include <iostream>
#include <random>

std::random_device rd; //��ʵ�����������

std::mt19937 mt(rd()); //���ɼ��������mt;

std::uniform_int_distribution<int> dist(-1000, 1000);//����-1000��1000֮�����ɢ���ȷֲ���

auto rnd = std::bind(dist, mt);

//�����߳�˯��ʱ��
void simulate_hard_computation() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000 + rnd()));
}

// ����������ֵļ򵥺�������ӡ���
void multiply(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
}

//��Ӳ�������
void multiply_output(int & out, const int a, const int b) {
    simulate_hard_computation();
    out = a * b;
    std::cout << a << " * " << b << " = " << out << std::endl;
}

// �������
int multiply_return(const int a, const int b) {
    simulate_hard_computation();
    const int res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
    return res;
}


void example() {
    // ����3���̵߳��̳߳�
    ThreadPool pool(3);

    // ��ʼ���̳߳�
    pool.init();

    // �ύ�˷��������ܹ�30��
    for (int i = 1; i < 3; ++i) {
        for (int j = 1; j < 10; ++j) {
            pool.submit(multiply, i, j);
        }
    }

    // ʹ��ref���ݵ���������ύ����
    int output_ref;
    auto future1 = pool.submit(multiply_output, std::ref(output_ref), 5, 6);

    // �ȴ��˷�������
    future1.get();
    std::cout << "Last operation result is equals to " << output_ref << std::endl;

    // ʹ��return�����ύ����
    auto future2 = pool.submit(multiply_return, 5, 3);

    // �ȴ��˷�������
    int res = future2.get();
    std::cout << "Last operation result is equals to " << res << std::endl;

    //�ر��̳߳�
    pool.shutdown();
}

int main()
{
    example();
    return 0;
}