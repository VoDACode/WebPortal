import os
import shutil
Import("env")

PACKAGE_NAME = "WebPortal"
DATA_SRC_NAME = "__wp"

global_env = DefaultEnvironment()

def uploadfs(source, target, env):
    print("Copy data files to project directory")
    project_dir = env.subst("$PROJECT_DIR")
    project_name = project_dir.split("\\")[-1]
    is_library = PACKAGE_NAME == project_name

    print("Project directory: " + project_dir)
    print("Project name: " + project_name)

    path_to_data = ""

    if is_library:
        print("This is a project")
        path_to_data = env.subst("$PROJECT_DIR") + os.sep + "src" + "data_src"
    else:
        print("This is a library")
        path_to_data = env.subst("$PROJECT_LIBDEPS_DIR") + os.sep + env.subst("$PIOENV") + os.sep + PACKAGE_NAME + os.sep + "src" + os.sep + "data_src"

    path_to_data += os.sep + DATA_SRC_NAME   
    print("Path to data: " + path_to_data)

    # copy content from path_to_data to project_dir + "/data". Replace files  - true
    if not os.path.exists(project_dir + os.sep + "data"):
        os.mkdir(project_dir + os.sep + "data")
        if os.path.exists(project_dir + os.sep + "data" + os.sep + DATA_SRC_NAME):
            os.removedirs(project_dir + os.sep + "data" + os.sep + DATA_SRC_NAME)
        os.mkdir(project_dir + os.sep + "data" + os.sep + DATA_SRC_NAME)
    shutil.copytree(path_to_data, project_dir + os.sep + "data" + os.sep + DATA_SRC_NAME, dirs_exist_ok=True)
    # upload fs
    env.Execute("pio run -t uploadfs")
    
global_env.AddPostAction("upload", uploadfs)

