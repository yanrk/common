#include "test_base.h"
#include "base_directory.h"

USING_NAMESPACE_COMMON

void test_base_directory()
{
    BASE_DIR base_dir;

    if (!base_opendir(".", base_dir))
    {
        return;
    }

    while (base_readdir(base_dir))
    {
        printf("%s%s\n", base_dir.file_name.c_str(), 
            (base_dir.file_is_dir ? " <dir>" : ""));
    }

    base_closedir(base_dir);
}
