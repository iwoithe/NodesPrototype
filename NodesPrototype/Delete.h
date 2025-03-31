#pragma once

#define DEL_STD_VEC(vec) for (int i = 0; i < vec.size(); i++) { \
    if (vec[i] != nullptr) { \
        delete vec[i]; \
    } \
} \
vec.clear()

#define DEL_PTR(ptr) if (ptr != nullptr) { delete ptr; }


/* Deleting std::set of pointers

for (auto it = m_ports.begin(); it != m_ports.end(); ++it)
{
    Port* port = *it;
    delete port;
    port = nullptr;
    it = m_ports.erase(it);
}

m_ports.clear();*/
