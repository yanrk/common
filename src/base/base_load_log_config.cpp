/********************************************************
 * Description : assistant function of load log config
 * Data        : 2013-08-04 16:35:54
 * Author      : yanrk
 * Email       : feeling_dxl@yeah.net & ken_scott@163.com
 * Blog        : blog.csdn.net/ken_scott
 * Version     : 1.0
 * History     :
 * Copyright(C): 2013 - 2015
 ********************************************************/

#include <string>

#include "base_utility.h"
#include "base_log_types.h"
#include "base_ini.h"

NAMESPACE_COMMON_BEGIN

static bool transform_log_write_mode(const std::string & key_value, 
                                     LOG_WRITE_MODE & write_mode)
{
    if ("SYNC_WRITE_MODE" == key_value)
    {
        write_mode = SYNC_WRITE_MODE;
    }
    else if ("ASYN_WRITE_MODE" == key_value)
    {
        write_mode = ASYN_WRITE_MODE;
    }
    else if ("LAZY_WRITE_MODE" == key_value)
    {
        write_mode = LAZY_WRITE_MODE;
    }
    else
    {
        return(false);
    }

    return(true);
}

static bool transform_log_level(const std::string & key_value, 
                                LOG_LEVEL & log_level)
{
    if ("CRI_LEVEL" == key_value)
    {
        log_level = CRI_LEVEL;
    }
    else if ("ERR_LEVEL" == key_value)
    {
        log_level = ERR_LEVEL;
    }
    else if ("WAR_LEVEL" == key_value)
    {
        log_level = WAR_LEVEL;
    }
    else if ("DBG_LEVEL" == key_value)
    {
        log_level = DBG_LEVEL;
    }
    else if ("TRK_LEVEL" == key_value)
    {
        log_level = TRK_LEVEL;
    }
    else
    {
        return(false);
    }

    return(true);
}

bool base_load_log_config(const std::string & file, 
                          LOG_CONFIG & log_config)
{
    BaseIni log_ini;
    if (!log_ini.load(file))
    {
        return(false);
    }

    if (!log_ini.get_value("", "log_path", log_config.log_file_path))
    {
        return(false);
    }

    for (int type = LOG_TYPE_MIN; type < LOG_TYPE_MAX; ++type)
    {
        std::string app_name(LOG_TYPE_INFO[type]);
        std::string key_value;

        if (!log_ini.get_value(app_name, "write_mode", key_value))
        {
            return(false);
        }

        if (!transform_log_write_mode(key_value, 
                                      log_config.log_file[type].write_mode))
        {
            return(false);
        }

        if (!log_ini.get_value(app_name, "min_level", key_value))
        {
            return(false);
        }

        if (!transform_log_level(key_value, 
                                 log_config.log_file[type].log_min_level))
        {
            return(false);
        }

        if (!log_ini.get_value(app_name, "file_size", 
                               log_config.log_file[type].log_file_size))
        {
            return(false);
        }

        if (!log_ini.get_value(app_name, "buffer_count", 
                               log_config.log_file[type].buffer_count))
        {
            return(false);
        }
    }

    return(true);
}

NAMESPACE_COMMON_END
