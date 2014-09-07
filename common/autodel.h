#pragma once
template<class T>
struct autodel
{   
   autodel(std::vector<T*>& v)
   {
       struct{ void operator() (T* cmd) { delete cmd; }} del;
       std::for_each(v.begin(), v.end(), del);
       v.clear();
   }
};

