cmake_minimum_required(VERSION 3.12)

project(MuduoDemo)

set(CMAKE_CXX_STANDARD 11)

# 设置 Muduo 库路径，根据你的实际安装路径进行修改
set(MUDUO_PATH "/usr/local/muduo")

include_directories(${MUDUO_PATH})

# server 服务器
add_executable(MuduoServer MuduoServer.cc)
target_link_libraries(MuduoServer /usr/local/lib/libmuduo_net.a
                                  /usr/local/lib/libmuduo_base.a pthread)

# client 客户端
add_executable(MuduoClient MuduoClient.cc)
target_link_libraries(MuduoClient /usr/local/lib/libmuduo_net.a
                                  /usr/local/lib/libmuduo_base.a pthread)