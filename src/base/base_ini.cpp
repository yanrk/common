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

#include <fstream>
#include <sstream>
#include <algorithm>

#include "base_ini.h"
#include "base_io.h"

NAMESPACE_COMMON_BEGIN

BaseIni::KEY_NODE::KEY_NODE(const std::string & name, const std::string & value)
    : m_name(name)
    , m_comment(false)
    , m_value(value)
{

}

bool BaseIni::KEY_NODE::operator == (const std::string & name) const
{
    return name == m_name;
}

BaseIni::APP_NODE::APP_NODE(const std::string & name)
    : m_name(name)
    , m_comment()
    , m_key_list()
{

}

bool BaseIni::APP_NODE::operator == (const std::string & name) const
{
    return name == m_name;
}

BaseIni::BaseIni()
    : m_file_name()
    , m_comment_char(';')
    , m_support_modify(false)
    , m_pair_map()
    , m_app_list()
    , m_last_comment()
{

}

BaseIni::~BaseIni()
{

}

bool BaseIni::load
(
    const std::string & file_name, 
    bool support_modify, 
    char comment_char
)
{
    clear();

    std::string app_name;
    std::list<std::string> comment;

    std::ifstream ifs(file_name.c_str());
    if (!ifs.is_open())
    {
        return false;
    }

    const int bufsiz = 4096;
    char buffer[bufsiz];
    while (!ifs.eof())
    {
        ifs.getline(buffer, bufsiz);
        std::string message(buffer);
        base_trim(message, ' ');
        base_trim(message, '\r');
        base_trim(message, '\n');

        if ((message.empty() && !comment.empty()) || 
            (!message.empty() && message[0] == comment_char))
        {
            if (support_modify)
            {
                comment.push_back(message);
            }
            continue;
        }

        if (message.empty())
        {
            continue;
        }

        if ('[' == message[0])
        {
            std::string::iterator b_iter = message.begin() + 1;
            std::string::iterator e_iter = message.end() - 1;
            if (']' != *e_iter)
            {
                e_iter = message.end();
            }
            std::string(b_iter, e_iter).swap(app_name);

            if (app_name.empty())
            {
                return false;
            }

            if (support_modify)
            {
                if (!add_app_node(app_name, comment))
                {
                    return false;
                }
            }

            comment.clear();
        }
        else
        {
            std::string::size_type pos = message.find('=');
            if (std::string::npos == pos)
            {
                return false;
            }

            std::string key_name(message.begin(), message.begin() + pos);
            std::string key_value(message.begin() + pos + 1, message.end());
            base_trim(key_name);
            base_trim(key_value);
            if (key_name.empty())
            {
                return false;
            }

            if (!m_pair_map.insert(
                    std::make_pair(std::make_pair(app_name, key_name), 
                                   key_value)).second)
            {
                return false;
            }

            if (support_modify)
            {
                if (app_name.empty() && m_app_list.empty())
                {
                    std::list<std::string> app_comment;
                    if (!add_app_node(app_name, app_comment))
                    {
                        return false;
                    }
                }

                if (!add_key_node(app_name, key_name, key_value, comment))
                {
                    return false;
                }
            }

            comment.clear();
        }
    }

    ifs.close();

    m_file_name = file_name;
    m_support_modify = support_modify, 
    m_comment_char = comment_char;
    m_last_comment.swap(comment);

    return true;
}

bool BaseIni::save()
{
    if (!m_support_modify)
    {
        return false;
    }

    const std::string tmp_file(m_file_name + "_tmp.ini");

    base_rename(m_file_name.c_str(), tmp_file.c_str());

    std::ofstream ofs(m_file_name, std::ios::trunc);
    if (!ofs.is_open())
    {
        return false;
    }

    APP_ITER app_iter = m_app_list.begin();
    while (m_app_list.end() != app_iter)
    {
        save_comment(ofs, app_iter->m_comment);
        save_app_name(ofs, app_iter->m_name);
        KEY_ITER key_iter = app_iter->m_key_list.begin();
        while (app_iter->m_key_list.end() != key_iter)
        {
            save_comment(ofs, key_iter->m_comment);
            save_key_value(ofs, key_iter->m_name, key_iter->m_value);
            ++key_iter;
        }
        ++app_iter;
    }
    save_comment(ofs, m_last_comment);

    ofs.close();

    base_unlink(tmp_file.c_str());

    return true;
}

bool BaseIni::get_value
(
    const std::string & app_name, 
    const std::string & key_name, 
    std::string & value
)
{
    if (key_name.empty())
    {
        return false;
    }

    PAIR_ITER pair_iter = m_pair_map.find(std::make_pair(app_name, key_name));
    if (m_pair_map.end() != pair_iter)
    {
        value = pair_iter->second;
        return true;
    }
    else
    {
        return false;
    }
}

bool BaseIni::set_value
(
    const std::string & app_name, 
    const std::string & key_name, 
    const std::string & value
)
{
    if (!m_support_modify)
    {
        return false;
    }

    if (key_name.empty())
    {
        return false;
    }

    APP_ITER app_iter = std::find(m_app_list.begin(), m_app_list.end(), app_name);
    if (m_app_list.end() == app_iter)
    {
        m_app_list.push_back(APP_NODE(app_name));
        app_iter = m_app_list.end();
        --app_iter;
    }
    std::list<KEY_NODE> & key_list = app_iter->m_key_list;
    KEY_ITER key_iter = std::find(key_list.begin(), key_list.end(), key_name);
    if (key_list.end() == key_iter)
    {
        key_list.push_back(KEY_NODE(key_name, value));
    }
    else
    {
        key_iter->m_value = value;
    }

    m_pair_map[std::make_pair(app_name, key_name)] = value;

    return true;
}

bool BaseIni::add_app_node
(
    const std::string & app_name, 
    std::list<std::string> & comment
)
{
    if (m_app_list.end() != 
        std::find(m_app_list.begin(), m_app_list.end(), app_name))
    {
        return false;
    }

    m_app_list.push_back(APP_NODE(app_name));
    APP_NODE & app_node = m_app_list.back();
    app_node.m_comment.swap(comment);
    return true;
}

bool BaseIni::add_key_node
(
    const std::string & app_name, 
    const std::string & key_name, 
    const std::string & key_value, 
    std::list<std::string> & comment
)
{
    APP_RE_ITER app_re_iter = 
        std::find(m_app_list.rbegin(), m_app_list.rend(), app_name);
    if (m_app_list.rend() == app_re_iter)
    {
        return false;
    }

    std::list<KEY_NODE> & key_list = app_re_iter->m_key_list;
    if (key_list.end() != 
        std::find(key_list.begin(), key_list.end(), key_name))
    {
        return false;
    }

    key_list.push_back(KEY_NODE(key_name, key_value));
    KEY_NODE & key_node = key_list.back();
    key_node.m_comment.swap(comment);
    return true;
}

void BaseIni::clear()
{
    m_file_name.clear();
    m_support_modify = false;
    m_comment_char = ';';
    m_pair_map.clear();
    m_app_list.clear();
}

void BaseIni::save_comment
(
    std::ofstream & ofs, 
    const std::list<std::string> & comment
)
{
    std::list<std::string>::const_iterator comment_iter = comment.begin();
    while (comment.end() != comment_iter)
    {
        ofs << (*comment_iter) << std::endl;
        ++comment_iter;
    }
}

void BaseIni::save_app_name
(
    std::ofstream & ofs, 
    const std::string & app
)
{
    if (!app.empty())
    {
        ofs << '[' << app << ']' << std::endl;
    }
}

void BaseIni::save_key_value
(
    std::ofstream & ofs, 
    const std::string & key, 
    const std::string & value
)
{
    ofs << key << '=' << value << std::endl;
}

NAMESPACE_COMMON_END
