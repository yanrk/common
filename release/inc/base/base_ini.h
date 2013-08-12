/********************************************************
 * Description : initial configuration class
 * Data        : 2013-07-27 16:11:54
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#ifndef COMMON_BASE_INI_H
#define COMMON_BASE_INI_H


#include <string>
#include <list>
#include <map>

#include "base_common.h"
#include "base_uncopy.h"
#include "base_convert.hpp"

NAMESPACE_COMMON_BEGIN

class YRK_EXPORT_DLL BaseIni : public BaseUncopy
{
public:
    BaseIni();
    ~BaseIni();

public:
    bool load(const std::string & file_name, 
              bool support_modify = false, 
              char comment_char = ';');
    bool save();
    void clear();

public:
    bool get_value(const std::string & app_name, const std::string & key_name, 
                   std::string & value);
    bool set_value(const std::string & app_name, const std::string & key_name, 
                   const std::string & value);

    template <typename T>
    bool get_value(const std::string & app_name, const std::string & key_name, 
                   T & value);

    template <typename T>
    bool set_value(const std::string & app_name, const std::string & key_name, 
                   T value);

private:
    bool add_app_node(const std::string & app_name, 
                      std::list<std::string> & comment);
    bool add_key_node(const std::string & app_name, 
                      const std::string & key_name, 
                      const std::string & key_value, 
                      std::list<std::string> & comment);
    void save_comment(std::ofstream & ofs, 
                      const std::list<std::string> & comment);
    void save_app_name(std::ofstream & ofs, 
                       const std::string & app);
    void save_key_value(std::ofstream & ofs, 
                        const std::string & key, 
                        const std::string & value);

private:
    struct KEY_NODE
    {
        std::string              m_name;
        std::list<std::string>   m_comment;
        std::string              m_value;

        KEY_NODE(const std::string & name);
        KEY_NODE(const std::string & name, const std::string & value);
        bool operator == (const std::string & name) const;
    };

    struct APP_NODE
    {
        std::string              m_name;
        std::list<std::string>   m_comment;
        std::list<KEY_NODE>      m_key_list;

        APP_NODE(const std::string & name);
        bool operator == (const std::string & name) const;
    };

    typedef std::pair<std::string, std::string> APP_KEY_PAIR;
    typedef std::map<APP_KEY_PAIR, std::string>::iterator PAIR_ITER;
    typedef std::list<APP_NODE>::iterator APP_ITER;
    typedef std::list<APP_NODE>::reverse_iterator APP_RE_ITER;
    typedef std::list<KEY_NODE>::iterator KEY_ITER;

private:
    std::string                           m_file_name;
    char                                  m_comment_char;
    bool                                  m_support_modify;
    std::map<APP_KEY_PAIR, std::string>   m_pair_map;
    std::list<APP_NODE>                   m_app_list;
    std::list<std::string>                m_last_comment;
};

template <typename T>
bool BaseIni::get_value(const std::string & app_name, 
                        const std::string & key_name, T & value)
{
    std::string key_value;
    if (!get_value(app_name, key_name, key_value))
    {
        return(false);
    }
    return(base_string2type(key_value, value));
}

template <typename T>
bool BaseIni::set_value(const std::string & app_name, 
                        const std::string & key_name, T value)
{
    std::string key_value;
    if (!base_type2string(value, key_value))
    {
        return(false);
    }
    return(set_value(app_name, key_name, key_value));
}

NAMESPACE_COMMON_END


#endif // COMMON_BASE_INI_H
