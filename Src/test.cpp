#include "Base.hpp"

void throwerr()
{
    throw std::runtime_error("´íÎó");
}

void nothing()
{
    throwerr();
}

int main(int args,char **arges)
{
    try
    {
        nothing();
    }
    catch (std::exception ex)
    {
        cout << ex.what() << endl;
    }
    catch (...)
    {
        cout << "sha\n";
    }
    return 0;
}