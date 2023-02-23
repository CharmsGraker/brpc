import os

rpc_header_path = None
runtime_class_header_path = None
runtime_class_cpp_path = None
auto_code_dir = "/home/nekonoyume/codeDev/CPlusPlus/buttonrpc_cpp14/auto-generated"
func_template = 'template<>\nstd::string getClassName({} *) {{\n\treturn \"{}\";\n}}'

runtime_template = """#include <string>
#include \"{}\"

template<class C>
std::string getClassName(C *) {{
    return "";
}}"""


def collect_all_rpc_class():
    global rpc_header_path, runtime_class_header_path, runtime_class_cpp_path

    class_collection = []
    assert auto_code_dir
    rpc_ns_path = os.path.join(auto_code_dir, "rpc_ns")
    rpc_header_path = os.path.join(auto_code_dir, "rpc_header.h")
    runtime_class_header_path = os.path.join(auto_code_dir, "runtime_class.h")
    runtime_class_cpp_path = os.path.join(auto_code_dir, "runtime_class.cpp")

    if os.path.exists(rpc_header_path):
        os.remove(rpc_header_path)

    with open(rpc_header_path, "w+") as f:
        if not os.path.exists(rpc_ns_path):
            return
        for root, dir, files in os.walk(rpc_ns_path):
            for fname in files:
                f.write("#include \"rpc_ns/{}\"\n".format(fname))
                classname = "".join(os.path.splitext(fname)[:-1])
                class_collection.append(classname)
    return class_collection


def add_runtime_class(classname):
    global rpc_header_path, runtime_class_header_path, runtime_class_cpp_path

    with open(runtime_class_cpp_path, 'a+') as f:
        func_str = func_template.format(classname, classname)
        f.write(func_str + "\n")


def add_include(f, header: str):
    f.write("#include\"{}\"\n".format(header))


def createFile(file_path, cover=True):
    if os.path.exists(file_path) and cover:
        os.remove(file_path)
    open(file_path, 'w')


def init_runtime_class():
    global rpc_header_path, runtime_class_header_path, runtime_class_cpp_path
    assert rpc_header_path
    assert runtime_class_header_path
    assert runtime_class_cpp_path
    createFile(runtime_class_header_path)
    createFile(runtime_class_cpp_path)

    with open(runtime_class_header_path, 'w') as f:
        f.write(runtime_template.format(rpc_header_path) + "\n")

    with open(runtime_class_cpp_path, 'w+') as f:
        add_include(f, "runtime_class.h")


if __name__ == '__main__':
    all_class = collect_all_rpc_class()
    init_runtime_class()
    for classname in all_class:
        print(f"collecting {classname}")
        add_runtime_class(classname)

print("all done.")
