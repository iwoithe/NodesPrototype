#pragma once

#include <any>

class Any
{
public:
    Any() = default;
    Any(std::any data)
    {
        m_data = data;
    }

    template<typename T> T get() const
    {
        return std::any_cast<T>(m_data);
    }
private:
    std::any m_data;
};
