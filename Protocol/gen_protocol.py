import sys
import argparse
import os
import subprocess
import re
import shutil

PROTOC_OUT_FOLDER = "gen/Private/pb"

def CHECK_FILE_EXIST(path):
    if not os.path.exists(path):
        print("File not exist: " + path)
        exit(1)
    return path

if __name__ == '__main__':
    print("Generate gRPC code..")
    parser = argparse.ArgumentParser(description="Generate gRPC wrapper for unreal.")
    parser.add_argument('folder', help="input folder which contains protocol files")
    parser.add_argument('--output_path', default="output", help="Output folder. By default gen/")
    args = parser.parse_args()

    CHECK_FILE_EXIST(args.folder)
    INCLUDE_PATH_PREFIX = "pb"
    BUF_GEN_TEMPLATE = "buf.gen.yaml"
    if args.folder == "spiceDB":
        INCLUDE_PATH_PREFIX = "pb/spiceDB"
        BUF_GEN_TEMPLATE = "spiceDB/buf.gen.yaml"

    if os.path.exists(PROTOC_OUT_FOLDER):
        print("Clean folder " + PROTOC_OUT_FOLDER)
        shutil.rmtree(PROTOC_OUT_FOLDER)

    print("Generate code at " + os.path.abspath(args.output_path))
    TL_UE_PLUGIN_PATH = CHECK_FILE_EXIST(os.path.abspath(os.path.join(os.path.dirname(__file__),"..")))
    FIX_PROTO_CPP = CHECK_FILE_EXIST(os.path.join(TL_UE_PLUGIN_PATH, 'Tools', 'fix_proto_cpp.txt'))
    FIX_PROTO_H = CHECK_FILE_EXIST(os.path.join(TL_UE_PLUGIN_PATH, 'Tools', 'fix_proto_h.txt'))
    BUF_EXE_PATH = CHECK_FILE_EXIST("buf.exe")
    
    subprocess.run([BUF_EXE_PATH,
        "generate",
        "--template",
        BUF_GEN_TEMPLATE,
        args.folder
        ], check=True)

    print("buf.exe generate Done")
    print("Apply patches to generated files...")

    with open(FIX_PROTO_H, "r") as f:
        fix_proto_h = f.read()
    with open(FIX_PROTO_CPP, "r") as f:
        fix_proto_cpp = f.read()
    
    for root, dirs, files in os.walk(PROTOC_OUT_FOLDER):
        for file_name in files:
            file_path = os.path.join(root, file_name)
            with open(file_path, "r") as f:
                file_content = f.read()
            file_content = re.sub(r'#include "(.*).pb.h"', r'#include "{0}/\1.pb.h"'.format(INCLUDE_PATH_PREFIX), file_content)
            if file_name.endswith("pb.h") and not file_name.endswith("grpc.pb.h"):
                file_content = fix_proto_h + file_content
            if file_name.endswith("pb.cc") and not file_name.endswith("grpc.pb.cc"):
                file_content = fix_proto_cpp + file_content
            if file_name == "debug.pb.h":
                file_content = "#ifdef check\n#undef check\n" + file_content + "#endif"
            with open(file_path, "w") as f:
                f.write(file_content)

    print("Code generation Done")